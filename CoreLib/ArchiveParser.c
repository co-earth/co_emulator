#include "Archive.h"
#include "FileIO.h"

typedef enum {
    TokenEndOfFile = 128,
    TokenIdentifier,
    TokenError,
} Token;

typedef struct {
    Archive* Archive;
    Int32 NodeIndex;
    Char* Start;
    Char* End;
    Char* Cursor;
    Token Token;
    Char* TokenStart;
    Char* TokenEnd;
    Token NextToken;
    Char* NextTokenStart;
    Char* NextTokenEnd;
    Bool NoLinebreak;
    Bool NoPeekNext;
    Bool NoCascade;
} ParseState;

static inline Void ParseNextToken(
    ParseState* State
);

static inline Bool CharacterIsSymbol(
    Char Character
) {
    switch (Character) {
    case '<':
    case '>':
    case '=':
    case '"':
    case '/':
        return true;

    default:
        return false;
    }
}

static inline Bool CharacterIsStartOfIdentifier(
    Char Character
) {
    return (
        'a' <= Character && Character <= 'z' ||
        'A' <= Character && Character <= 'Z' ||
        '_' == Character
        );
}

static inline Bool CharacterIsContinuationOfIdentifier(
    Char Character
) {
    return (
        'a' <= Character && Character <= 'z' ||
        'A' <= Character && Character <= 'Z' ||
        '0' <= Character && Character <= '9' ||
        '_' == Character
        );
}

static inline Bool CharacterIsWhitespace(
    Char Character
) {
    switch (Character) {
    case 0x09:
    case 0x20:
        return true;

    default:
        return false;
    }
}

static inline Bool CharacterIsLinebreak(
    Char Character
) {
    switch (Character) {
    case 0x0A:
    case 0x0B:
    case 0x0C:
    case '\r':
        return true;

    default:
        return false;
    }
}

static inline Void SkipUntilCharacter(
    ParseState* state,
    Char terminator
) {
    if (*state->TokenStart == terminator)
        return;

    Char* start = state->Cursor;

    while (state->Cursor < state->End) {
        if (state->NoLinebreak && CharacterIsLinebreak(*state->Cursor)) {
            break;
        }

        if (*state->Cursor == terminator) {
            break;
        }

        state->Cursor += 1;
    }

    ParseNextToken(state);
}

static inline Bool SkipSeparator(
    ParseState* state
) {
    Char* start = state->Cursor;

    while (state->Cursor < state->End) {
        if (!(CharacterIsWhitespace(*state->Cursor)) &&
            !(CharacterIsLinebreak(*state->Cursor) && !state->NoLinebreak)) {
            break;
        }

        state->Cursor += 1;
    }

    return start != state->Cursor;
}

static inline Bool ConsumeToken(ParseState* state, Token token) {
    if (state->Token == token) {
        state->Cursor = state->TokenEnd;
        ParseNextToken(state);
        return true;
    }

    return false;
}

static inline Void ParseNextToken(
    ParseState* state
) {
    SkipSeparator(state);

    if (state->Cursor >= state->End) {
        state->Token = TokenEndOfFile;
        state->TokenStart = state->Cursor;
        state->TokenEnd = state->Cursor + 1;
        goto end;
    }

    if (CharacterIsSymbol(*state->Cursor)) {
        state->Token = (Token)*state->Cursor;
        state->TokenStart = state->Cursor;
        state->TokenEnd = state->Cursor + 1;
        state->Cursor += 1;
        goto end;
    }

    if (CharacterIsStartOfIdentifier(*state->Cursor)) {
        Char* start = state->Cursor;
        do {
            state->Cursor += 1;
        } while (CharacterIsContinuationOfIdentifier(*state->Cursor) && state->Cursor < state->End);

        state->Token = TokenIdentifier;
        state->TokenStart = start;
        state->TokenEnd = state->Cursor;
        goto end;
    }

    state->Token = TokenError;
    state->TokenStart = state->Cursor;
    state->TokenEnd = state->Cursor + 1;
    goto end;

end:
    if (!state->NoPeekNext) {
        ParseState copy = *state;
        copy.NoPeekNext = true;
        ParseNextToken(&copy);
        state->NextToken = copy.Token;
        state->NextTokenStart = copy.TokenStart;
        state->NextTokenEnd = copy.TokenEnd;
    }
}

static inline Bool TryParseAttribute(
    ParseState* State,
    Bool Append
) {
    ParseState CurrentState = *State;

    Char* AttributeStart = CurrentState.TokenStart;
    Char* AttributeEnd = CurrentState.TokenEnd;

    if (!ConsumeToken(&CurrentState, TokenIdentifier)) goto error;

    if (!ConsumeToken(&CurrentState, '=')) goto error;

    Char* LiteralStart = CurrentState.TokenStart;
    Bool NoLinebreak = CurrentState.NoLinebreak;
    CurrentState.NoLinebreak = true;

    if (!ConsumeToken(&CurrentState, '"')) goto error;

    SkipUntilCharacter(&CurrentState, '"');

    Char* LiteralEnd = CurrentState.TokenEnd;

    if (!ConsumeToken(&CurrentState, '"')) goto error;

    CurrentState.NoLinebreak = NoLinebreak;

    ParseState NextState = CurrentState;
    NextState.NoCascade = true;
    if (!TryParseAttribute(&NextState, false)) {
        if (CurrentState.Token == '>' ||
            (CurrentState.Token == '/' && CurrentState.NextToken == '>')) {
            if (Append) {
                ArchiveNodeAddAttribute(
                    CurrentState.Archive,
                    CurrentState.NodeIndex,
                    AttributeStart,
                    AttributeEnd - AttributeStart,
                    LiteralStart + 1,
                    LiteralEnd - LiteralStart - 2
                );
            }

            *State = CurrentState;
            return true;
        }

        NoLinebreak = CurrentState.NoLinebreak;
        CurrentState.NoLinebreak = true;

        SkipUntilCharacter(&CurrentState, '"');

        LiteralEnd = CurrentState.TokenEnd;

        if (!ConsumeToken(&CurrentState, '"')) goto error;

        CurrentState.NoLinebreak = NoLinebreak;
    }

    if (Append) {
        ArchiveNodeAddAttribute(
            CurrentState.Archive,
            CurrentState.NodeIndex,
            AttributeStart,
            AttributeEnd - AttributeStart,
            LiteralStart + 1,
            LiteralEnd - LiteralStart - 2
        );
    }

    *State = CurrentState;
    return true;

error:
    return false;
}

Bool ArchiveParseFromSource(
    Archive* Archive,
    CString Source,
    Int32 Length,
    Bool IgnoreErrors
) {
    ParseState State = { 0 };
    State.Start = Source;
    State.End = Source + Length;
    State.Cursor = Source;
    State.Archive = Archive;
    State.NodeIndex = -1;

    ParseNextToken(&State);

    while (State.Token != TokenEndOfFile && State.Token != TokenError) {
        if (!ConsumeToken(&State, '<')) 
            goto error;

        if (ConsumeToken(&State, '/')) {
            Char* NameStart = State.TokenStart;
            Char* NameEnd = State.TokenEnd;

            if (!ConsumeToken(&State, TokenIdentifier)) goto error;
            if (!ConsumeToken(&State, '>')) goto error;

            if (State.NodeIndex < 0)
                goto error;

            ArchiveString* Name = ArchiveNodeGetName(Archive, State.NodeIndex);
            if (!IgnoreErrors && memcmp(Name->Data, NameStart, NameEnd - NameStart) != 0)
                goto error;

            State.NodeIndex = ArchiveNodeGetParent(Archive, State.NodeIndex);
            continue;
        }

        Char* NameStart = State.TokenStart;
        Char* NameEnd = State.TokenEnd;

        State.NodeIndex = ArchiveAddNode(Archive, State.NodeIndex, NameStart, NameEnd - NameStart);

        if (!ConsumeToken(&State, TokenIdentifier)) 
            goto error;

        while (State.Token == TokenIdentifier) {
            if (!TryParseAttribute(&State, true)) 
                goto error;
        }

        if (ConsumeToken(&State, '>')) {
            ParseState CurrentState = State;
            if (ConsumeToken(&CurrentState, '/') && ConsumeToken(&CurrentState, '/')) {
                SkipUntilCharacter(&CurrentState, '\n');
                State = CurrentState;
            }

            continue;
        }

        ParseState CurrentState = State;
        if (ConsumeToken(&CurrentState, '/') && ConsumeToken(&CurrentState, '>')) {
            if (CurrentState.NodeIndex < 0)
                goto error;

            CurrentState.NodeIndex = ArchiveNodeGetParent(Archive, CurrentState.NodeIndex);
            State = CurrentState;
            continue;
        }

        goto error;
    }

    return true;

error:
    return false;
}
