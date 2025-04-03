const std = @import("std");

const print = std.debug.print;
const posix = std.posix;
const linux = std.os.linux;
const ascii = std.ascii;
const mem = std.mem;
const io = std.io;

const Buflen = 1000;

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

    loop: while (true) {
        @memset(&buffer, 0); // Clear the buffer

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

        for (buffer[0..buflen], 0..) |char, idx| {
            if (ascii.isAlphabetic(char)) {
                print("\nExpression can not contain a alphabetical character\n", .{});
                print("{s}\n", .{buffer});

                for (0..idx) |_| print("-", .{});
                print("^ contains an alphabetic charactre\n", .{});

                continue :loop;
            }
        }

        print("Got exp: {s}\n", .{buffer});
    }
}
