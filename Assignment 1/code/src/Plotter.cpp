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

void runPlotterScript(istream& input) {
    string line;
    while (getline(input, line)) {
        try {
            Command command = parseLine(line);
            // Here you would pass the command to the appropriate handler
            // For demonstration purposes, we'll just print out the command type
            if (holds_alternative<PenDown>(command)) {
                cout << "PenDown command" << endl;
            } else if (holds_alternative<PenUp>(command)) {
                cout << "PenUp command" << endl;
            } else if (holds_alternative<MoveAbs>(command)) {
                auto cmd = get<MoveAbs>(command);
                cout << "MoveAbs command to (" << cmd.x << ", " << cmd.y << ")" << endl;
            } else if (holds_alternative<MoveRel>(command)) {
                auto cmd = get<MoveRel>(command);
                cout << "MoveRel command by (" << cmd.dx << ", " << cmd.dy << ")" << endl;
            } else if (holds_alternative<PenColor>(command)) {
                auto cmd = get<PenColor>(command);
                cout << "PenColor command with color " << cmd.color << endl;
            } else if (holds_alternative<PenWidth>(command)) {
                auto cmd = get<PenWidth>(command);
                cout << "PenWidth command with width " << cmd.width << endl;
            }
        } catch (const exception& e) {
            cerr << "Error: " << e.what() << endl;
        }
    }
}
