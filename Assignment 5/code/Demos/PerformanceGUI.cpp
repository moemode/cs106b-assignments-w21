#include "GUI/MiniGUI.h"
#include "Combine.h"
#include "BigOFunctions.h"
#include "ginteractors.h"
#include "gtimer.h"
#include "simpio.h"
#include <functional>
#include <chrono>
#include <cstdint>
#include <string>
#include <limits>
#include <cmath>
#include <iomanip>
using namespace std;
using namespace MiniGUI;

namespace {
    const string kNotSelected = "-";

    /* Graphics constants. */
    const double kEventTimerSpeed = 1; // Fast enough for things to run smoothly, not enough to drown us
    const string kBackgroundColor = "White";
    const double kPadding = 20;

    /* Height of the console, as a fraction of the canvas size. */
    const double kConsoleScaleFactor = 0.9;

    /* Axis constants. */
    const char kAxisColor[] = "#555555"; // Davy's gray
    const Font kAxisFont(FontFamily::SERIF, FontStyle::NORMAL, 8, kAxisColor);
    const size_t kMaxAxisLabels = 10;    // Max number of major tick marks on the X axis

    /* Content areas. */
    const double kHeaderHeight    = 50;
    const Font kHeaderFont(FontFamily::SERIF, FontStyle::BOLD_ITALIC, 24, kAxisColor);

    /* Legend area, expressed as an offset from the start of the chart area. */
    const double kLegendXOffset   = 100;
    const double kLegendYOffset   =   0;
    const double kLegendWidth     = 150;
    const double kLegendHeight    = 200;
    const Font kLegendFont(FontFamily::SANS_SERIF, FontStyle::NORMAL, 12, kAxisColor);


    /**
     * A type representing a stopwatch. This is used to time the costs of various
     * operations.
     */
    class Timer {
    public:
      void start() {
        current = chrono::high_resolution_clock::now();
      }

      void stop() {
        total += chrono::high_resolution_clock::now() - current;
      }

      double elapsed() const {
        return chrono::duration_cast<chrono::nanoseconds>(total).count();
      }

    private:
      chrono::high_resolution_clock::duration total =
        chrono::high_resolution_clock::duration::zero();
      chrono::high_resolution_clock::time_point current;
    };

    /* All the information needed to run a time test. */
    struct TimeTest {
        function<void (size_t n, size_t k, Timer& timer)> callback; // Given n, k, and a timer, do work.
        vector<size_t> ns, ks;                                      // Values of n and k
        string name;                                                // What we're testing
    };

    /* Generates numbers in linear and exponential sequences */
    vector<size_t> linRange(size_t low, size_t high, size_t step) {
        vector<size_t> result;
        for (; low <= high; low += step) {
            result.push_back(low);
        }
        return result;
    }

    vector<size_t> expRange(size_t low, size_t high, double step) {
        vector<size_t> result;
        for (; low <= high; low *= step) {
            result.push_back(low);
        }
        return result;
    }

    /* Divert output from cout somewhere else. */
    struct StreamDiverter {
        ostringstream buffer;
        streambuf* oldBuf;
        StreamDiverter() {
            oldBuf = cout.rdbuf(buffer.rdbuf());
        }

        ~StreamDiverter() {
            cout.rdbuf(oldBuf);
        }
    };

    /* To make sure the compiler doesn't optimize out some of these calls,
     * we increment the accumulator with the result of those calls.
     */
    volatile unsigned int accumulator = 0;

    /* List of all the different tests we can run. */
    const vector<TimeTest> kAllTests = {
        /* printH */
        {
            [](int n, int, Timer& timer) {
                StreamDiverter diverter;

                timer.start();
                printH(n);
                timer.stop();
            },
            linRange(1000, 10000, 1000),
            { 1 },
            "printH"
        },

        /* printC */
        {
            [](int n, int, Timer& timer) {
                StreamDiverter diverter;

                timer.start();
                printC(n);
                timer.stop();
            },
            linRange(100000, 1000000, 50000),
            { 1 },
            "printC"
        },

        /* printI */
        {
            [](int n, int, Timer& timer) {
                StreamDiverter diverter;

                timer.start();
                printI(n);
                timer.stop();
            },
            linRange(1000, 10000, 1000),
            { 1 },
            "printI"
        },

        /* printP */
        {
            [](int n, int, Timer& timer) {
                StreamDiverter diverter;

                timer.start();
                printP(n);
                timer.stop();
            },
            linRange(1000, 10000, 1000),
            { 1 },
            "printP"
        },

        /* printChip */
        {
            [](int n, int, Timer& timer) {
                StreamDiverter diverter;

                timer.start();
                printChip(n);
                timer.stop();
            },
            linRange(1000, 10000, 1000),
            { 1 },
            "printChip"
        },

        /* countTriples */
        {
            [](int n, int, Timer& timer) {
                Vector<int> values;
                for (int i = 0; i < n; i++) {
                    values += randomInteger(-1000, +1000);
                }

                timer.start();
                accumulator += countTriples(values, 137);
                timer.stop();
            },
            linRange(100, 1000, 100),
            { 1 },
            "countTriples"
        },

        /* printCycle_v1 */
        {
            [](int n, int, Timer& timer) {
                StreamDiverter diverter;

                Vector<int> values;
                for (int i = 0; i < n; i++) {
                    values += randomInteger(-1000, +1000);
                }

                timer.start();
                printCycle_v1(values);
                timer.stop();
            },
            linRange(10000, 50000, 5000),
            { 1 },
            "printCycle_v1"
        },

        /* printCycle_v2 */
        {
            [](int n, int, Timer& timer) {
                StreamDiverter diverter;

                Vector<int> values;
                for (int i = 0; i < n; i++) {
                    values += randomInteger(-1000, +1000);
                }

                timer.start();
                printCycle_v2(values);
                timer.stop();
            },
            linRange(1000, 5000, 500),
            { 1 },
            "printCycle_v2"
        },

        /* printCycle_v3 */
        {
            [](int n, int, Timer& timer) {
                StreamDiverter diverter;

                Queue<int> values;
                for (int i = 0; i < n; i++) {
                    values.enqueue(randomInteger(-1000, +1000));
                }

                timer.start();
                printCycle_v3(values);
                timer.stop();
            },
            linRange(1000000, 5000000, 500000),
            { 1 },
            "printCycle_v3"
        },

        /* recursivePuzzle */
        {
            [](int n, int, Timer& timer) {
                timer.start();
                for (int i = 0; i < 10000; i++) {
                    recursivePuzzle(n);
                }
                timer.stop();
            },
            linRange(1000, 5000, 500),
            { 1 },
            "recursivePuzzle"
        },

        /* recursiveEnigma */
        {
            [](int n, int, Timer& timer) {
                timer.start();
                for (int i = 0; i < 1000000; i++) {
                    recursiveEnigma(n);
                }
                timer.stop();
            },
            linRange(0, 100000000, 1000000),
            { 1 },
            "recursiveEnigma"
        },

        /* maximumSingleSellProfit_v1 */
        {
            [](int n, int, Timer& timer) {
                Vector<int> values;
                for (int i = 0; i < n; i++) {
                    values += randomInteger(-1000, +1000);
                }

                timer.start();
                accumulator += maximumSingleSellProfit_v1(values);
                timer.stop();
            },
            linRange(1000, 10000, 1000),
            { 1 },
            "maximumSingleSellProfit_v1"
        },

        /* maximumSingleSellProfit_v2 */
        {
            [](int n, int, Timer& timer) {
                Vector<int> values;
                for (int i = 0; i < n; i++) {
                    values += randomInteger(-1000, +1000);
                }

                timer.start();
                accumulator += maximumSingleSellProfit_v2(values);
                timer.stop();
            },
            linRange(10000, 100000, 10000),
            { 1 },
            "maximumSingleSellProfit_v2"
        },

        /* Combine */
        {
            [](int n, int k, Timer& timer) {
                /* Make a bunch of vectors to merge. */
                Vector<Vector<DataPoint>> toMerge(k);

                /* Add elements in following a mostly geometric distribution. */
                for (int i = 0; i < n; i++) {
                    int index = 0;
                    while (index < k - 1 && randomBool()) index++;

                    toMerge[index] += { to_string(i), i };
                }

                /* Run the multiway merge. */
                timer.start();
                (void) combine(toMerge);
                timer.stop();
            },
            linRange(10000, 100000, 5000),
            expRange(1, 1 << 12, 4),
            "combine"
        }
    };

    /* Test driver, factored out from any particular method of displaying the test results. */
    class TestDriver {
    public:
        TestDriver(const TimeTest& test,
                   function<void(int k)> onNewK,
                   function<void(int n, double value)> onNewN,
                   function<void()> onStop) : test(test),
                                              onNewK(onNewK),
                                              onNewN(onNewN),
                                              onStop(onStop) {

        }

        /* Runs the next step of the test. This is done so that the GUI can use
         * a timer callback and the test driver code can just step until done.
         */
        void step();

    private:
        TimeTest test;
        size_t nextN = 0, nextK = 0; // Next values of n and k to use.

        function<void(int k)> onNewK;
        function<void(int n, double value)> onNewN;
        function<void()> onStop;
    };

    void TestDriver::step() {
        /* Are we at the very beginning? */
        if (nextN == 0 && nextK == 0) {
            onNewK(test.ks[nextK]);
        }

        /* Otherwise, run the next test. */
        Timer timer;
        test.callback(test.ns[nextN], test.ks[nextK], timer);

        /* Store the result. */
        onNewN(test.ns[nextN], timer.elapsed());

        /* Step forward. */
        nextN++;

        /* Finished all n's? Advance k. */
        if (nextN == test.ns.size()) {
            nextN = 0;
            nextK++;

            /* Finished all k's? We're done! */
            if (nextK == test.ks.size()) {
                onStop();
                return;
            } else {
                onNewK(test.ks[nextK]);
            }
        }
    }

    /* Colors to use for various values of k. */
    const vector<string> kLineColors = {
        "#CC0000", // Rosso Corsa
        "#EE7F2D", // Princeton Orange
        "#FFC40C", // Mikado Yellow
        "#008000", // Office Green
        "#007BA7", // Cerulean
        "#B53389", // Fandango
        "#343434", // Jet
    };

    /* Problem handler that reports timing information for the different types. */
    class PerformanceGUI: public ProblemHandler {
    public:
        PerformanceGUI(GWindow& window);

        void timerFired() override;
        void actionPerformed(GObservable* source) override;

    protected:
        void repaint() override;

    private:
        /* Each timer tick steps us through the next test case. */
        GTimer timer{kEventTimerSpeed};

        /* Which test, if any, we're doing. */
        bool isActive = false;
        TimeTest currTest;

        vector<string> axisLabels{2};   // Labels to use on the x axis. Defaults to two blank strings
        size_t axisTicks = 0;           // Minor ticks to use on the x axis

        /* Test case results so far. */
        unique_ptr<TestDriver> driver;
        Vector<Vector<double>> results;

        /* Chrome layout:
         *
         * +------------------------------+
         * | Function: [ dropdown] [Go!]  |
         * +------------------------------+
         */
        Temporary<GLabel> functionLabel;
        Temporary<GComboBox> functionSelector;
        Temporary<GButton> go;

        Map<string, const TimeTest*> tests;

        /* Side console. */
        Temporary<GColorConsole> console;

        void startTests(const TimeTest& test);

        void drawChart (GWindow& window, const GRectangle& bounds);
        void drawHeader(GWindow& window, const GRectangle& bounds);
        void drawLegend(GWindow& window, const GRectangle& bounds);
    };

    PerformanceGUI::PerformanceGUI(GWindow& window) : ProblemHandler(window) {
        console = make_temporary<GColorConsole>(window, "WEST");
        console->setHeight(window.getCanvasHeight() * kConsoleScaleFactor);

        functionLabel = make_temporary<GLabel>(window, "SOUTH", "Function: ");
        functionSelector = make_temporary<GComboBox>(window, "SOUTH");
        functionSelector->addItem(kNotSelected);

        for (const auto& test: kAllTests) {
            tests[test.name] = &test;
            functionSelector->addItem(test.name);
        }

        go = make_temporary<GButton>(window, "SOUTH", "Time");
    }

    void PerformanceGUI::timerFired() {
        /* If we don't have an active test, there's nothing to do. */
        if (!isActive) {
            timer.stop();
            return;
        }

        driver->step();

        /* Need to redraw things! */
        requestRepaint();
    }

    /* Begins a new testing session. */
    void PerformanceGUI::startTests(const TimeTest& test) {
        /* Clear out all old test information. */
        currTest = test;

        /* Map from k values to text colors. This is ignored if there
         * is just one k value.
         */
        map<size_t, string> colors;
        for (size_t k: test.ks) {
            colors.insert(make_pair(k, kLineColors[colors.size()]));
        }

        /* Reset state. */
        auto onNewK = [test, colors, this](int k) {
            /* Add an new place for the results to go. */
            results.add({});

            /* Display that choice of k if there are multiple k's to go around. */
            if (test.ks.size() != 1) {
                console->doWithStyle(colors.at(k), GColorConsole::BOLD, [&]{
                    *console << "k = " << k << endl;
                });
            }
        };

        auto onNewN = [this](int n, double elapsed) {
            /* Store the result. */
            // results.last() += elapsed;
            results[results.size() - 1] += elapsed;

            *console << "  n = " << left << setw(10) << (to_string(n) + ": ") << elapsed / 1e6 << "ms" << endl;
        };

        auto onStop = [this] {
            timer.stop();
            isActive = false;
        };

        driver.reset(new TestDriver(test, onNewK, onNewN, onStop));
        results.clear();
        console->clearDisplay();

        /* Set up x-axis labels. */
        axisLabels.clear();

        /* We will never have more than kMaxAxisLabels labels on the X axis. If we
         * do, we will start introducing minor tickmarks.
         *
         * The way we're going to do this is as follows. We'll compute
         * ceil(#labels / kAxisLabels), which gives back the number of tick skips
         * to use. For example, if we want 11 labels and the max is 10, we'll compute
         * ceil(11 / 10) - 1 = 2, and have one minor tick in between each set
         * of major ticks.
         *
         * We actually compute this using the technique that (a + b - 1) / b computes
         * ceil(a / b).
         */
        axisTicks = ((test.ns.size() + kMaxAxisLabels - 1) / kMaxAxisLabels);
        size_t numMajor = (test.ns.size() + axisTicks - 1) / axisTicks;

        for (size_t i = 0; i < numMajor; i++) {
            size_t effectiveIndex = i * axisTicks; // We're skipping things, after all!

            string label;
            if (effectiveIndex == 0) {
                label = "n=" + to_string(test.ns[effectiveIndex]);
            } else if (effectiveIndex < test.ns.size()) {
                label = to_string(test.ns[effectiveIndex]);
            } else {
                label = ""; // Not needed, but helps clarify intent.
            }

            axisLabels.push_back(label);
        }

        /* The axisTicks parameter actually literally controls how many minor ticks are
         * drawn, not the skip size between major ticks. As a result, we need to subtract
         * one here in order to ensure that we draw teh right number of minor ticks.
         */
        axisTicks--;

        isActive = true;
        timer.start();

        console->doWithStyle(FontSize(16), [&] {
            *console << test.name << endl;
        });
    }


    void PerformanceGUI::actionPerformed(GObservable* source) {
        if (source == go) {
            string choice = functionSelector->getSelectedItem();
            if (choice != kNotSelected && tests.containsKey(choice)) {
                startTests(*tests[choice]);
            }
        }
    }

    void PerformanceGUI::repaint() {
        clearDisplay(window(), kBackgroundColor);

        /* Space for the header. */
        GRectangle header = {
            kPadding, kPadding,
            window().getCanvasWidth() - 2 * kPadding,
            kHeaderHeight
        };

        double chartTop = header.y + header.height;

        /* Set up the chart. */
        GRectangle chart = {
            kPadding, chartTop,
            window().getCanvasWidth()  - 2 * kPadding,
            window().getCanvasHeight() - chartTop - kPadding
        };

        /* And the legend. */
        GRectangle legend = {
            chart.x + kLegendXOffset, chart.y + kLegendYOffset,
            kLegendWidth, kLegendHeight
        };

        drawHeader(window(), header);
        drawChart (window(), chart);
        drawLegend(window(), legend);
    }

    void PerformanceGUI::drawHeader(GWindow& window, const GRectangle& bounds) {
        auto render = TextRender::construct(currTest.name, bounds, kHeaderFont, LineBreak::NO_BREAK_SPACES);
        render->alignCenterHorizontally();
        render->alignCenterVertically();
        render->draw(window);
    }

    void PerformanceGUI::drawChart(GWindow& window, const GRectangle& bounds) {
        /* Y-axis labels will be empty. We need a minimum of two, though, so we'll make those. */
        vector<string> yLabels(2);

        /* Convert our results into line segments. */

        /* Find the maximum value across everything. */
        double maxValue = 0;
        double minValue = numeric_limits<double>::infinity();
        for (const auto& result: results) {
            for (double val: result) {
                maxValue = max(val, maxValue);
                minValue = min(val, minValue);
            }
        }

        /* Nudge the max value up a bit to avoid dividing by zero. */
        maxValue = nextafter(maxValue, numeric_limits<double>::infinity());

        /* Normalize everything by that amount. */
        vector<vector<GPoint>> lines;
        for (const auto& result: results) {
            vector<GPoint> line;
            for (int i = 0; i < result.size(); i++) {
                line.push_back({ i * 1.0 / (currTest.ns.size() - 1), (result[i] - minValue) / (maxValue - minValue) });
            }
            lines.push_back(line);
        }

        /* Draw it! */
        LineGraphRender::construct(lines,
                                   axisLabels,
                                   yLabels,
                                   axisTicks,
                                   0,
                                   bounds,
                                   kAxisFont,
                                   kAxisFont,
                                   kLineColors,
                                   kAxisColor)->draw(window);
    }

    void PerformanceGUI::drawLegend(GWindow& window, const GRectangle& bounds) {
        /* If there is only one value of k, don't draw anything. */
        if (currTest.ks.size() <= 1) return;

        /* Otherwise, there are many values of k. Assemble labels based on the number of lines
         * we have drawn so far.
         */
        vector<string> labels;
        for (size_t i = 0; i < currTest.ks.size() && i < size_t(results.size()); i++) {
            labels.push_back("k = " + to_string(currTest.ks[i]));
        }

        /* Set up the legend. Don't draw it yet; we need to clear the area behind it. */
        auto legend = LegendRender::construct(labels, kLineColors, bounds, kLegendFont, kLegendFont.color());
        window.setColor(kBackgroundColor);
        window.fillRect(legend->computedBounds());

        /* Now draw it. */
        legend->draw(window);
    }
}

GRAPHICS_HANDLER("Time Tests", GWindow& window) {
    return make_shared<PerformanceGUI>(window);
}

CONSOLE_HANDLER("Time Tests") {
    Vector<string> options;

    for (const auto& entry: kAllTests) {
        options += entry.name;
    }

    do {
        int choice = makeSelectionFrom("Choose a function to time-test:", options);
        auto test = kAllTests[choice];

        auto onNextK = [&](int k) {
            if (test.ks.size() != 1) {
                cout << "k = " << k << endl;
            }
        };

        auto onNextN = [&](int n, double result) {
            cout << "  n = " << left << setw(10) << (to_string(n) + ": ") << result / 1e6 << "ms" << endl;
        };

        bool done = false;
        auto onDone = [&] {
            done = true;
        };

        cout << "Timing " << test.name << endl;
        TestDriver driver(test, onNextK, onNextN, onDone);
        while (!done) {
            driver.step();
        }
    } while (getYesOrNo("Run tests on another function? "));
}
