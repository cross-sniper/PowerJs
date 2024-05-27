function main() {
    e = std.atoi("30");
    print(e);
    print(std.typeof(e));
    e = std.itoa(30);
    print(e);
    print(std.typeof(e));
    print(std.format("this is a %s", "string")); // Use %s as the placeholder
    player = {
        x: 0,
        y: 0
    };
    print(player);
}
