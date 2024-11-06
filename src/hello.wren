var module_name = "hello"
import "test" for testvar
import "test"

System.print(testvar + module_name)

class Wren {
    construct new() {}
    flyTo(city) {
        System.print("Flying to %(city)")
    }
}

var adjectives = Fiber.new {
    ["small", "clean", "fast"].each {|word|
        // System.print(word + " inside fiber")
        Fiber.yield(word)
    }
}

while (!adjectives.isDone) {
    var ret = adjectives.call()
    if (ret) System.print(ret)
}

var wren = Wren.new()
wren.flyTo("Stockholm")
