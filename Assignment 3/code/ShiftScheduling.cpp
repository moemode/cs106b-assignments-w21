#include "ShiftScheduling.h"
#include "error.h"
using namespace std;


/**
 * Calculates the total value of the given set of shifts.
 *
 * @param shifts The set of shifts for which to calculate the total value.
 * @return The total value of all shifts in the set.
 */
int valueOfShifts(const Set<Shift> & shifts) {
    int totalValue = 0;
    for (const Shift & shift : shifts) {
        totalValue += valueOf(shift); // Assuming Shift has a method value() that returns its value
    }
    return totalValue;
}

/**
 * Recursively computes the highest value schedule of non-overlapping shifts within the given time constraints.
 *
 * This function uses a recursive approach to evaluate two main scenarios for each shift:
 * 1. Including the current shift in the schedule.
 * 2. Excluding the current shift from the schedule.
 *
 * It considers the constraints of remaining hours and ensures that shifts do not overlap. The function selects
 * the schedule with the highest total value by comparing the results of the two scenarios.
 *
 * @param sortedShifts A vector of shifts sorted by day and start hour. This sorting helps in efficiently
 *                     skipping overlapping shifts.
 * @param currentShiftIndex The index of the shift currently being considered for inclusion in the schedule.
 * @param remainingHours The total number of hours left for scheduling. This parameter helps ensure that
 *                       the total length of selected shifts does not exceed the allowed hours.
 *
 * @return A set of `Shift` objects representing the optimal schedule with the highest total value, ensuring
 *         non-overlapping shifts and adherence to the remaining hours constraint. If no shifts can be included
 *         within the remaining hours, the function will return an empty set.
 *
 * @note The function assumes that shifts are pre-sorted to simplify the overlap checking process.
 *
 * @see valueOfShifts
 * @see overlapsWith
 * @see lengthOf
 */
Set<Shift> highestValueScheduleForRec(const vector<Shift>& sortedShifts, int currentShiftIndex, int remainingHours) {
    // Base case: If all shifts have been processed, return an empty set
    if (currentShiftIndex >= sortedShifts.size()) {
        return {};
    }
    Shift curr = sortedShifts[currentShiftIndex];
    // If the current shift is too long to fit within the remaining hours, skip it
    if (lengthOf(curr) > remainingHours) {
        return highestValueScheduleForRec(sortedShifts, currentShiftIndex + 1, remainingHours);
    }
    // Compute the best schedule including the current shift
    // - Move to the next non-overlapping shift
    // - Reduce the remaining hours by the length of the current shift
    int firstNonOverlapping = currentShiftIndex + 1;
    // Skip over any shifts that overlap with the current shift
    while (firstNonOverlapping < sortedShifts.size() && overlapsWith(curr, sortedShifts[firstNonOverlapping])) {
        ++firstNonOverlapping;
    }
    Set<Shift> best_with = highestValueScheduleForRec(sortedShifts, firstNonOverlapping, remainingHours - lengthOf(curr));
    best_with.add(curr); // Include the current shift in the result

    // Compute the best schedule without including the current shift
    Set<Shift> best_without = highestValueScheduleForRec(sortedShifts, currentShiftIndex + 1, remainingHours);
    if (valueOfShifts(best_with) > valueOfShifts(best_without)) {
        return best_with;
    }
    return best_without;
}

/**
 * Computes the highest value schedule of non-overlapping shifts that fits within a specified number of hours.
 *
 * This function prepares the set of shifts by sorting them according to their day and start hour. It then invokes
 * a recursive function to find the optimal schedule. The scheduling is done under the constraint of not exceeding
 * the given maximum hours and ensuring that no shifts overlap.
 *
 * @param shifts A set of `Shift` objects representing all available shifts. The function will process these shifts
 *               to determine the optimal schedule.
 * @param maxHours The maximum number of hours that the total schedule can encompass. This parameter ensures that
 *                 the sum of hours of the selected shifts does not exceed this limit.
 *
 * @return A set of `Shift` objects representing the optimal schedule with the highest total value. The result will
 *         respect the maximum hours constraint and ensure that selected shifts do not overlap. If no shifts can be
 *         selected under the given constraint, the function will return an empty set.
 *
 * @throws error if `maxHours` is negative. The function expects a non-negative value for `maxHours`.
 *
 * @note The function sorts the shifts by day and start hour before proceeding with the scheduling. This sorting
 *       helps streamline the process of checking for overlapping shifts.
 *
 * @see highestValueScheduleForRec
 * @see valueOf
 * @see lengthOf
 * @see overlapsWith
 */
Set<Shift> highestValueScheduleFor(const Set<Shift>& shifts, int maxHours) {
    if(maxHours < 0) {
        error("Must not have negative maxHours");
    }
    vector<Shift> remainingShifts;
    for (const Shift& shift : shifts) {
        remainingShifts.push_back(shift);
    }
    // Sort shifts by day and startHour lexicographically
    sort(remainingShifts.begin(), remainingShifts.end(), [](const Shift& a, const Shift& b) {
        if (a.day != b.day) return a.day < b.day;
        return a.startHour < b.startHour;
    });
    return highestValueScheduleForRec(remainingShifts, 0, maxHours);
}



/* * * * * * Test Cases * * * * * */
#include "GUI/SimpleTest.h"

/* TODO: Add your own tests here. You know the drill - look for edge cases, think about
 * very small and very large cases, etc.
 */

STUDENT_TEST("Basic test with non-overlapping shifts") {
    Set<Shift> shifts = {
        {Day::MONDAY, 9, 12, 10},
        {Day::MONDAY, 13, 16, 15},
        {Day::TUESDAY, 9, 12, 8},
        {Day::WEDNESDAY, 9, 12, 12}
    };
    int maxHours = 8;

    Set<Shift> expected = { {Day::MONDAY, 13, 16, 15}, {Day::WEDNESDAY, 9, 12, 12} };

    Set<Shift> result = highestValueScheduleFor(shifts, maxHours);
    EXPECT_EQUAL(result, expected);
}

STUDENT_TEST("All shifts overlap") {
    Set<Shift> shifts = {
        {Day::MONDAY, 9, 12, 10},
        {Day::MONDAY, 11, 14, 15},
        {Day::MONDAY, 11, 16, 20}
    };
    int maxHours = 6;

    // Since all shifts overlap, only one can be selected. The one with the highest value is: { {MONDAY, 13, 16, 20} }
    Set<Shift> expected = { {Day::MONDAY, 11, 16, 20} };

    Set<Shift> result = highestValueScheduleFor(shifts, maxHours);
    EXPECT_EQUAL(result, expected);
}


STUDENT_TEST("Max hours exceed total available hours") {
    Set<Shift> shifts = {
        {Day::MONDAY, 9, 12, 10},
        {Day::TUESDAY, 10, 13, 20}
    };
    int maxHours = 10;

    // Both shifts can be selected since their total duration (6 hours + 3 hours) is less than maxHours (10).
    Set<Shift> expected = {
        {Day::MONDAY, 9, 12, 10},
        {Day::TUESDAY, 10, 13, 20}
    };

    Set<Shift> result = highestValueScheduleFor(shifts, maxHours);
    EXPECT_EQUAL(result, expected);
}


STUDENT_TEST("Empty set of shifts") {
    Set<Shift> shifts;
    int maxHours = 8;

    // With no shifts available, the result should be an empty set.
    Set<Shift> expected;

    Set<Shift> result = highestValueScheduleFor(shifts, maxHours);
    EXPECT_EQUAL(result, expected);
}








/* * * * * * Test cases from the starter files below this point. * * * * * */
#include "vector.h"
#include "error.h"

/* This nice utility function lets you call highestValueScheduleFor, passing in
 * a Vector of shifts rather than a Set. This makes it a bit easier to test things.
 * You shouldn't need this function outside of these test cases.
 */
Set<Shift> asSet(const Vector<Shift>& shifts) {
    Set<Shift> result;
    for (Shift s: shifts) {
        result += s;
    }
    return result;
}

PROVIDED_TEST("Pick only shift if you have time for it.") {
    Set<Shift> shifts = {
        { Day::MONDAY, 9, 17, 1000 },  // Monday, 9AM - 5PM, value is 1000
    };

    EXPECT_EQUAL(highestValueScheduleFor(shifts, 24).size(), 1);
    EXPECT_EQUAL(highestValueScheduleFor(shifts, 24), shifts);
}

PROVIDED_TEST("Don't pick only shift if ou don't have time for it.") {
    Set<Shift> shifts = {
        { Day::MONDAY, 9, 17, 1000 },  // Monday, 9AM - 5PM, value is 1000
    };

    EXPECT_EQUAL(highestValueScheduleFor(shifts, 3).size(), 0);
}

PROVIDED_TEST("Don't pick overlapping shifts.") {
    Vector<Shift> shifts = {
        { Day::MONDAY, 9, 17, 1000 },  // Monday,  9AM - 5PM, value is 1000
        { Day::MONDAY, 8, 18, 2000 },  // Monday, 10AM - 6PM, value is 2000
    };

    EXPECT_EQUAL(highestValueScheduleFor(asSet(shifts), 100), { shifts[1] });
}

PROVIDED_TEST("Doesn't always use highest-value shift.") {
    Vector<Shift> shifts = {
        { Day::MONDAY,    10, 20, 1000 }, // 10-hour shift, value is 1000
        { Day::TUESDAY,   10, 15,  500 }, //  5-hour shift, value is 500
        { Day::WEDNESDAY, 10, 16,  501 }, //  6-hour shift, value is 501
    };

    /* The correct strategy is to forgo the highest-value shift in favor of the two
     * shorter shifts.
     */
    auto schedule = highestValueScheduleFor(asSet(shifts), 11);

    EXPECT_EQUAL(schedule, { shifts[1], shifts[2] });
}

PROVIDED_TEST("Doesn't always use the shift with the highest value per unit time.") {
    Vector<Shift> shifts = {
        { Day::MONDAY,    10, 17, 21 }, //  7-hour shift, value is 21 ($3 / hour)
        { Day::TUESDAY,   10, 16, 12 }, //  6-hour shift, value is 12 ($2 / hour)
        { Day::WEDNESDAY, 10, 16, 12 }, //  6-hour shift, value is 12 ($2 / hour)
    };

    /* If you have 12 hours, the correct strategy is to pick the two six-hour shifts
     * for a total of $24 value. Picking the shift with the highest value per unit
     * time (the seven-hour shift) produces only $21 value.
     */
    auto schedule = highestValueScheduleFor(asSet(shifts), 12);

    EXPECT_EQUAL(schedule, { shifts[1], shifts[2] });
}

PROVIDED_TEST("Passes the example from the assignment description.") {
    Vector<Shift> shifts = {
        { Day::MONDAY,     8, 12, 27 },  // Mon  8AM - 12PM, value 27 *
        { Day::MONDAY,    12, 16, 28 },  // Mon 12PM -  4PM, value 28 *
        { Day::MONDAY,    16, 20, 25 },  // Mon  4PM -  8PM, value 25 *
        { Day::MONDAY,     8, 14, 39 },  // Mon  8AM -  2PM, value 39
        { Day::MONDAY,    14, 20, 31 },  // Mon  2PM -  8PM, value 31
        { Day::TUESDAY,    8, 12,  7 },  // Tue  8AM - 12PM, value  7
        { Day::TUESDAY,   12, 16,  7 },  // Tue 12PM -  4PM, value  7
        { Day::TUESDAY,   16, 20, 11 },  // Tue  4PM -  8PM, value 11
        { Day::TUESDAY,    8, 14, 10 },  // Tue  8AM -  2PM, value 10
        { Day::TUESDAY,   14, 20,  8 },  // Tue  2PM -  8PM, value  8
        { Day::WEDNESDAY,  8, 12, 10 },  // Wed  8AM - 12PM, value 10
        { Day::WEDNESDAY, 12, 16, 11 },  // Wed 12PM -  4PM, value 11
        { Day::WEDNESDAY, 16, 20, 13 },  // Wed  4PM -  8PM, value 13
        { Day::WEDNESDAY,  8, 14, 19 },  // Wed  8AM -  2PM, value 19
        { Day::WEDNESDAY, 14, 20, 25 },  // Wed  2PM -  8PM, value 25 *
    };

    /* Get back the solution. */
    Set<Shift> computedSolution = highestValueScheduleFor(asSet(shifts), 20);

    /* Form the correct answer. It's the starred entries. */
    Set<Shift> actualSolution = {
        shifts[0], shifts[1], shifts[2], shifts[14]
    };

    EXPECT_EQUAL(computedSolution, actualSolution);
}

PROVIDED_TEST("Handles no shifts.") {
    EXPECT_EQUAL(highestValueScheduleFor({}, 137).size(), 0);
}

PROVIDED_TEST("Reports an error with negative hours.") {
    /* From the assignment description. */
    Vector<Shift> shifts = {
        { Day::MONDAY,     8, 12, 27 },  // Mon  8AM - 12PM, value 27
        { Day::MONDAY,    12, 16, 28 },  // Mon 12PM -  4PM, value 28
        { Day::MONDAY,    16, 20, 25 },  // Mon  4PM -  8PM, value 25
        { Day::MONDAY,     8, 14, 39 },  // Mon  8AM -  2PM, value 39
        { Day::MONDAY,    14, 20, 31 },  // Mon  2PM -  8PM, value 31
        { Day::TUESDAY,    8, 12,  7 },  // Tue  8AM - 12PM, value  7
        { Day::TUESDAY,   12, 16,  7 },  // Tue 12PM -  4PM, value  7
        { Day::TUESDAY,   16, 20, 11 },  // Tue  4PM -  8PM, value 11
        { Day::TUESDAY,    8, 14, 10 },  // Tue  8AM -  2PM, value 10
        { Day::TUESDAY,   14, 20,  8 },  // Tue  2PM -  8PM, value  8
        { Day::WEDNESDAY,  8, 12, 10 },  // Wed  8AM - 12PM, value 10
        { Day::WEDNESDAY, 12, 16, 11 },  // Wed 12PM -  4PM, value 11
        { Day::WEDNESDAY, 16, 20, 13 },  // Wed  4PM -  8PM, value 13
        { Day::WEDNESDAY,  8, 14, 19 },  // Wed  8AM -  2PM, value 19
        { Day::WEDNESDAY, 14, 20, 25 },  // Wed  2PM -  8PM, value 25
    };

    /* Should be an error. */
    EXPECT_ERROR(highestValueScheduleFor(asSet(shifts), -1));

    /* Still an error even if there are no shifts. */
    EXPECT_ERROR(highestValueScheduleFor({}, -1));
}

PROVIDED_TEST("Handles zero free hours.") {
    /* From the assignment description. */
    Vector<Shift> shifts = {
        { Day::MONDAY,     8, 12, 27 },  // Mon  8AM - 12PM, value 27
        { Day::MONDAY,    12, 16, 28 },  // Mon 12PM -  4PM, value 28
        { Day::MONDAY,    16, 20, 25 },  // Mon  4PM -  8PM, value 25
        { Day::MONDAY,     8, 14, 39 },  // Mon  8AM -  2PM, value 39
        { Day::MONDAY,    14, 20, 31 },  // Mon  2PM -  8PM, value 31
        { Day::TUESDAY,    8, 12,  7 },  // Tue  8AM - 12PM, value  7
        { Day::TUESDAY,   12, 16,  7 },  // Tue 12PM -  4PM, value  7
        { Day::TUESDAY,   16, 20, 11 },  // Tue  4PM -  8PM, value 11
        { Day::TUESDAY,    8, 14, 10 },  // Tue  8AM -  2PM, value 10
        { Day::TUESDAY,   14, 20,  8 },  // Tue  2PM -  8PM, value  8
        { Day::WEDNESDAY,  8, 12, 10 },  // Wed  8AM - 12PM, value 10
        { Day::WEDNESDAY, 12, 16, 11 },  // Wed 12PM -  4PM, value 11
        { Day::WEDNESDAY, 16, 20, 13 },  // Wed  4PM -  8PM, value 13
        { Day::WEDNESDAY,  8, 14, 19 },  // Wed  8AM -  2PM, value 19
        { Day::WEDNESDAY, 14, 20, 25 },  // Wed  2PM -  8PM, value 25
    };

    /* Shouldn't be an error if time is zero - that means we just don't pick anything. */
    EXPECT_EQUAL(highestValueScheduleFor(asSet(shifts), 0).size(), 0);
}

PROVIDED_TEST("Stress test: Don't generate shift combinations with overlapping shifts.") {
    /* All of these shifts overlap one another. If you try producing all combinations
     * of these shifts and only check at the end whether they're valid, you'll be
     * checking 2^100 ~= 10^30 combinations of shifts, which will take so long the
     * sun will have burnt out before you're finished.
     *
     * Instead, as you're going through your decision tree and building up your shifts,
     * make sure not to include any shifts that clearly conflict with something you
     * picked earlier.
     */
    Set<Shift> trickySet;
    for (int i = 0; i < 100; i++) {
        trickySet += Shift{ Day::MONDAY, 1, 2, i };
    }
    EXPECT_EQUAL(trickySet.size(), 100);

    auto result = highestValueScheduleFor(trickySet, 1);
    EXPECT_EQUAL(result.size(), 1);
}

PROVIDED_TEST("Stress test: Don't generate shift combinations that exceed time limits.") {
    /* Here's a collection of one shift per hour of the week. Your worker has exactly
     * one hour free. If you try all possible combinations of these shifts, ignoring time
     * constraints, you will have to check over 2^100 = 10^30 combinations, which will
     * take longer than the length of the known universe to process.
     *
     * Instead, as you're exploring the decision tree to generate shift combinations,
     * make sure not to add shifts that would exceed the time limit.
     */
    Set<Shift> trickySet;
    for (Day day: { Day::SUNDAY,
                    Day::MONDAY,
                    Day::TUESDAY,
                    Day::WEDNESDAY,
                    Day::THURSDAY,
                    Day::FRIDAY,
                    Day::SATURDAY}) {
        for (int start = 0; start < 24; start++) {
            trickySet += Shift{ day, start, start + 1, 10 };
        }
    }
    EXPECT_EQUAL(trickySet.size(), 7 * 24);

    auto result = highestValueScheduleFor(trickySet, 1);
    EXPECT_EQUAL(result.size(), 1);
}

PROVIDED_TEST("Stress test: Can handle a decent number of shifts (should take at most 10-15 seconds)") {
    /* Available shifts. */
    Vector<Shift> shifts = {
        { Day::SUNDAY,  8, 14, 12 },
        { Day::SUNDAY, 12, 18, 36 },

        { Day::MONDAY,  8, 12, 44 },
        { Day::MONDAY, 12, 16, 32 },
        { Day::MONDAY, 16, 20,  0 },
        { Day::MONDAY,  8, 16, 16 },
        { Day::MONDAY, 12, 20, 22 },

        { Day::TUESDAY,  8, 12, 48 },
        { Day::TUESDAY, 12, 16, 20 },
        { Day::TUESDAY, 16, 20, 24 },
        { Day::TUESDAY,  8, 16, 24 },
        { Day::TUESDAY, 12, 20, 80 },

        { Day::WEDNESDAY,  8, 12, 20 },
        { Day::WEDNESDAY, 12, 16,  8 },
        { Day::WEDNESDAY, 16, 20,  8 },
        { Day::WEDNESDAY,  8, 16, 40 },
        { Day::WEDNESDAY, 12, 20, 16 },

        { Day::THURSDAY,  8, 12, 40 },
        { Day::THURSDAY, 12, 16,  0 },
        { Day::THURSDAY, 16, 20, 24 },
        { Day::THURSDAY,  8, 16, 56 },
        { Day::THURSDAY, 12, 20, 32 },

        { Day::FRIDAY,  8, 12,  4 },
        { Day::FRIDAY, 12, 16,  8 },
        { Day::FRIDAY, 16, 20, 40 },
        { Day::FRIDAY,  8, 16, 72 },
        { Day::FRIDAY, 12, 20, 40 },

        { Day::SATURDAY,  8, 14, 18 },
        { Day::SATURDAY, 12, 18, 66 },
    };

    auto answer = highestValueScheduleFor(asSet(shifts), 30);
    EXPECT_EQUAL(answer, { shifts[2], shifts[7], shifts[11], shifts[17], shifts[24], shifts[28] });
}
