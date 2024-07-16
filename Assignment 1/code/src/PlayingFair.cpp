/* File: PlayingFair.cpp
 *
 * TODO: Edit these comments to describe anything interesting or noteworthy in your implementation.
 */
#include "PlayingFair.h"
#include "GUI/SimpleTest.h"
#include "error.h"
using namespace std;

string aSequenceOfOrder(int n) {
    if(n < 0) {
        error("Error: Sequence length is negative (" + std::to_string(n) + "). It must be >= 0.");
    }
    if(n == 0) {
        return "A";
    }
    return aSequenceOfOrder(n - 1) + bSequenceOfOrder(n - 1);
}

string bSequenceOfOrder(int n) {
    if(n < 0) {
        error("Error: Sequence length is negative (" + std::to_string(n) + "). It must be >= 0.");
    }
    if(n == 0) {
        return "B";
    }
    return bSequenceOfOrder(n - 1) + aSequenceOfOrder(n - 1);
}






/* * * * * * Provided Test Cases * * * * * */

PROVIDED_TEST("Sequences of order 3 are correct.") {
    /* Some very basic checks. */
    EXPECT_EQUAL(aSequenceOfOrder(3), "ABBABAAB");
    EXPECT_EQUAL(bSequenceOfOrder(3), "BAABABBA");
}

PROVIDED_TEST("Only characters should be As and Bs.") {
    for (int i = 0; i < 10; i++) {
        for (char ch: aSequenceOfOrder(i)) {
            EXPECT(ch == 'A' || ch == 'B');
        }
    }
}

PROVIDED_TEST("A-sequence of positive order should have equal As and Bs.") {
    /* Exclude the sequence of order 0, which is just a single character. */
    for (int i = 1; i < 10; i++) {
        int as = 0;
        int bs = 0;
        for (char ch: aSequenceOfOrder(i)) {
            if (ch == 'A') as++;
            else bs++;
        }

        EXPECT_EQUAL(as, bs);
    }
}

PROVIDED_TEST("Triggers error on negative inputs.") {
    /* The EXPECT_ERROR macro expects the given expression to call error(). Remember that
     * you need to guard against invalid inputs.
     */
    EXPECT_ERROR(aSequenceOfOrder(-137));
    EXPECT_ERROR(bSequenceOfOrder(-137));
}

/* TODO: You will need to add your own tests into this suite of test cases. Think about the sorts
 * of inputs we tested here, and, importantly, what sorts of inputs we *didn't* test here. Some
 * general rules of testing:
 *
 *    1. Try extreme cases. What are some very large cases to check? What are some very small cases?
 *
 *    2. Be diverse. There are a lot of possible inputs out there. Make sure you have tests that account
 *       for cases that aren't just variations of one another.
 *
 *    3. Be sneaky. Don't just try standard inputs. Try weird ones that you wouldn't expect anyone to
 *       actually enter, but which are still perfectly legal.
 *
 * Happy testing!
 */
STUDENT_TEST("Base case i.e. sequences of order 0 are correct.") {
    EXPECT_EQUAL(aSequenceOfOrder(0), "A");
    EXPECT_EQUAL(bSequenceOfOrder(0), "B");
}

STUDENT_TEST("Test sequences of order from 0 to 20 for the concatenation property.") {
    const int maxOrder = 20;
    string aSeqs[maxOrder + 1];
    string bSeqs[maxOrder + 1];

    for (int n = 0; n <= maxOrder; ++n) {
        aSeqs[n] = aSequenceOfOrder(n);
        bSeqs[n] = bSequenceOfOrder(n);
    }

    for (int n = 1; n <= maxOrder; ++n) {
        EXPECT_EQUAL(aSeqs[n], aSeqs[n - 1] + bSeqs[n - 1]);
        EXPECT_EQUAL(bSeqs[n], bSeqs[n - 1] + aSeqs[n - 1]);
    }
}






