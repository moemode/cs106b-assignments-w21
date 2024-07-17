#include "Plotter.h"
#include "strlib.h" // For stringSplit
using namespace std;

class PenDown {
};

class PenUp {
};

class MoveAbs {
public:
    double x, y;
    MoveAbs(double x, double y) : x(x), y(y) {}
};

class MoveRel {
public:
    double dx, dy;
    MoveRel(double dx, double dy) : dx(dx), dy(dy) {}
};

class PenColor {
public:
    string color;
    PenColor(const string& color) : color(color) {}
};

class PenWidth {
public:
    double width;
    PenWidth(double width) : width(width) {}
};

using Command = variant<PenDown, PenUp, MoveAbs, MoveRel, PenColor, PenWidth>;


string toLower(const string& str) {
    string lowerStr = str;
    transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(), ::tolower);
    return lowerStr;
}

Command parseLine(const string& line) {
    auto parts = stringSplit(line, " ");
    if (parts.isEmpty()) {
        throw invalid_argument("Empty command line");
    }
    string commandType = toLower(parts[0]);
    if (commandType == "pendown") {
        return PenDown();
    } else if (commandType == "penup") {
        return PenUp();
    } else if (commandType == "moveabs") {
        if (parts.size() != 3) {
            throw invalid_argument("MoveAbs command requires 2 arguments");
        }
        double x = stod(parts[1]);
        double y = stod(parts[2]);
        return MoveAbs(x, y);
    } else if (commandType == "moverel") {
        if (parts.size() != 3) {
            throw invalid_argument("MoveRel command requires 2 arguments");
        }
        double dx = stod(parts[1]);
        double dy = stod(parts[2]);
        return MoveRel(dx, dy);
    } else if (commandType == "pencolor") {
        if (parts.size() != 2) {
            throw invalid_argument("PenColor command requires 1 argument");
        }
        return PenColor(parts[1]);
    } else if (commandType == "penwidth") {
        if (parts.size() != 2) {
            throw invalid_argument("PenWidth command requires 1 argument");
        }
        double width = stod(parts[1]);
        return PenWidth(width);
    } else {
        throw invalid_argument("Unknown command type: " + parts[0]);
    }
}

struct CommandExecutor {
    bool penDown = false;
    double currentX = 0.0;
    double currentY = 0.0;
    PenStyle currentStyle = {1.0, "black"};
    void (*drawLineFunc)(double, double, double, double, PenStyle);

    CommandExecutor(void (*drawLineFunc)(double, double, double, double, PenStyle))
        : drawLineFunc(drawLineFunc) {}


    void operator()(const PenDown& cmd) {
        penDown = true;
        cout << "Pen down" << endl;
    }

    void operator()(const PenUp& cmd) {
        penDown = false;
        cout << "Pen up" << endl;
    }

    void operator()(const MoveAbs& cmd) {
        if (penDown) {
            drawLine(currentX, currentY, cmd.x, cmd.y, currentStyle);
        }
        currentX = cmd.x;
        currentY = cmd.y;
        cout << "Moved to (" << currentX << ", " << currentY << ")" << endl;
    }

    void operator()(const MoveRel& cmd) {
        double newX = currentX + cmd.dx;
        double newY = currentY + cmd.dy;
        if (penDown) {
            drawLine(currentX, currentY, newX, newY, currentStyle);
        }
        currentX = newX;
        currentY = newY;
        cout << "Moved to (" << currentX << ", " << currentY << ")" << endl;
    }

    void operator()(const PenColor& cmd) {
        currentStyle.color = cmd.color;
        cout << "Changed color to " << currentStyle.color << endl;
    }

    void operator()(const PenWidth& cmd) {
        currentStyle.width = cmd.width;
        cout << "Changed pen width to " << currentStyle.width << endl;
    }
};

void executeCommand(const Command& command, CommandExecutor& executor) {
    std::visit(executor, command);
}

void runPlotterScript(istream& input) {

    CommandExecutor executor(drawLine);
    for (string line; getline(input, line); ) {
        try {
            Command command = parseLine(line);
            executeCommand(command, executor);
        } catch (const exception& e) {
            cerr << "Error: " << e.what() << endl;
        }
    }
}
