const std = @import("std");

const print = std.debug.print;
const posix = std.posix;
const linux = std.os.linux;
const ascii = std.ascii;
const mem = std.mem;
const io = std.io;

const Buflen = 1000;
const Numlen = 100;

const Operator = enum(u8) {
    Plus,
    Minus,
    Multiply,
    Devide,
    Unknown
};

fn is_operator(char: u8) bool {
    if (assign_operator(char) == .Unknown) return false;
    return true;
}

fn assign_operator(char: u8) Operator {
    return switch (char) {
        '+' => .Plus,
        '-' => .Minus,
        '*' => .Multiply,
        '/' => .Devide,
        else => .Unknown
    };
}

fn strlen(string: []const u8) usize {
    var count: usize = 0;
    while (count < string.len) {
        if (string[count] == 0) break;
        count += 1;
    }

    return count;
}

fn strcmp(this: []const u8, that: []const u8) bool {
    if (strlen(this) != strlen(that)) return false;

    for (0..strlen(this)) |i| {
        if (this[i] != that[i]) return false;
    }

    return true;
}

pub fn main() !void {
    var buffer: [Buflen]u8 = undefined;
    var lhs: [Numlen]u8 = undefined;
    var rhs: [Numlen]u8 = undefined;

    loop: while (true) {
        @memset(&buffer, 0); // Reset the buffer
        @memset(&lhs, 0);
        @memset(&rhs, 0);

        print("> ", .{});
        _ = linux.read(posix.STDIN_FILENO, &buffer, Buflen-1);
        var buflen = strlen(&buffer);

        if (buffer[buflen-1] == '\n') {
            buffer[buflen-1] = 0;
            buflen -= 1;
        }

        if (buflen <= 1) {
            print("Expression is too short or invalid!\n", .{});
            continue;
        }

        if (strcmp(&buffer, "quit") or strcmp(&buffer, "exit")) {
            print("Goodbye\n", .{});
            return;
        }

        if (strcmp(&buffer, "clear")) {
            print("\x1b[2J\x1b[H", .{});
            continue;
        }

        for (buffer[0..buflen], 0..) |char, idx| {
            if (ascii.isAlphabetic(char)) {
                print("\nExpression can not contain a alphabetical character\n", .{});
                print("{s}\n", .{buffer});

                for (0..idx) |_| print("-", .{});
                print("^ contains an alphabetic charactre\n", .{});

                continue :loop;
            }

            if (!ascii.isDigit(char) and !is_operator(char)) {
                print("\nExpression contains a invalid/unknown character!\n", .{});
                print("{s}\n", .{buffer});

                for (0..idx) |_| print("-", .{});
                print("^ contains an alphabetic charactre\n", .{});

                continue :loop;
            }
        }

        var op: Operator = undefined;
        for (buffer[0..buflen]) |_| {
            var i: usize = 0;
            while(!is_operator(buffer[i])) {
                lhs[i] = buffer[i];
                i += 1;
            }

            op = assign_operator(buffer[i]);
            i += 1;

            var x: usize = 0;
            while(i < buflen) {
                rhs[x] = buffer[i];
                i += 1;
                x += 1;
            }

            break; // @Temporary: Don't parse the rest of the expression 
        }

        print("LHS: {s} and RHS: {s} and operator = {}\n", .{lhs, rhs, op});
    }
}
