import type { SchemaPositions } from "../types/one";

const escapePointerSegment = (segment: string): string =>
  segment.replace(/~/g, "~0").replace(/\//g, "~1");

export const computeJsonPositions = (text: string): SchemaPositions => {
  const positions: SchemaPositions = {};
  let i = 0;
  let line = 0;
  let col = 0;

  const advance = () => {
    if (text[i] === "\n") {
      line++;
      col = 0;
    } else {
      col++;
    }
    i++;
  };

  const skipWhitespace = () => {
    while (i < text.length && /\s/.test(text[i])) advance();
  };

  const parseString = (): string => {
    let result = "";
    advance();
    while (i < text.length && text[i] !== '"') {
      if (text[i] === "\\") {
        advance();
        result += text[i];
        advance();
      } else {
        result += text[i];
        advance();
      }
    }
    advance();
    return result;
  };

  const parseValue = (pointer: string): void => {
    skipWhitespace();
    const startLine = line;
    const startCol = col;
    const ch = text[i];

    if (ch === "{") {
      advance();
      skipWhitespace();
      if (text[i] === "}") {
        advance();
      } else {
        for (;;) {
          skipWhitespace();
          const key = parseString();
          skipWhitespace();
          advance();
          parseValue(`${pointer}/${escapePointerSegment(key)}`);
          skipWhitespace();
          if (text[i] === ",") {
            advance();
            continue;
          }
          break;
        }
        skipWhitespace();
        advance();
      }
    } else if (ch === "[") {
      advance();
      skipWhitespace();
      if (text[i] === "]") {
        advance();
      } else {
        let index = 0;
        for (;;) {
          parseValue(`${pointer}/${index}`);
          index++;
          skipWhitespace();
          if (text[i] === ",") {
            advance();
            continue;
          }
          break;
        }
        skipWhitespace();
        advance();
      }
    } else if (ch === '"') {
      parseString();
    } else {
      while (i < text.length && !/[,}\]\s]/.test(text[i])) advance();
    }

    positions[pointer] = [startLine, startCol, line, col];
  };

  parseValue("");
  return positions;
};
