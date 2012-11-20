import sys


colors = map(lambda c: "\x1b[%im" % c, range(31, 38))
bold = "\x1b[1m"
end = "\x1b[0m"

if __name__ == '__main__':
    assert len(sys.argv) > 1, "Se debe espeficificar el nombre de los procesos que se quiere tracear."
    assert len(sys.argv) < len(colors),  "Hay demasiados nombres, no se podra distingirlos a todos." 
    expecteds = sys.argv[1:]
    color_by_process = dict(zip(expecteds, colors))

    for process, color in color_by_process.iteritems():
        print " -", color, process, end

    print
    for line in sys.stdin:
        _, _, time, _, process = line.split()[:5]
        process = process[:-1]
        try:
            process_name, _ = process.split("[")[:2]
        except:
            process_name = process
        msg = " ".join(line.split()[5:])

        best = ""
        for prefix in expecteds:
            if process_name.startswith(prefix) and len(best) < len(prefix):
                best = prefix
        
        if not best:
           continue

        color = color_by_process[best]
        high = bold if any(map(lambda tag: msg.startswith(tag), ["[Emerg]", "[Alert]", "[Crit]", "[Err]", "[Warning]", "[Notice]"])) else ""

        print time, color, process, high, msg, end
