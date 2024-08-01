#include "WhatAreYouDoing.h"
#include "strlib.h"
using namespace std;

/**
 * Checks if a given string is considered a word.
 *
 * A string is classified as a word if it is non-empty and its first character is an alphabetical character.
 *
 * @param s The string to be checked.
 * @return True if the string is a word, false otherwise.
 */
bool isWord(const string& s) {
    return !s.empty() && isalpha(s[0]);
}

/**
 * Recursively generates all possible capitalizations of the words in the sentence.
 *
 * This function uses recursion to explore all possible combinations of capitalizations for each word in
 * the sentence. It processes each token in the sentence and combines them with the results from previous
 * recursive calls.
 *
 * @param sentence The Vector<string> containing the tokens of the sentence.
 * @param wordIndex The index of the current word to process in the Vector<string>.
 * @return A Set<Vector<string>> where each Vector<string> represents a variant of the sentence with
 *         different capitalizations applied to the words.
 */
Set<Vector<string>> allEmphasesOfRec(Vector<string> & sentence, int wordIndex) {
    if(wordIndex < 0) {
        Set<Vector<string>> empty_vector;
        empty_vector.add({});
        return empty_vector;
    }
    Set<Vector<string>> prefixEmphases = allEmphasesOfRec(sentence, wordIndex - 1);
    string token = sentence[wordIndex];
    vector<string> wordForms;
    if(isWord(token)) {
        wordForms = {toUpperCase(token), toLowerCase(token)};
    } else {
        wordForms = {token};
    }
    Set<Vector<string>> emphases;
    for (const string& form : wordForms) {
        for(const Vector<string> & prefix: prefixEmphases) {
            Vector<string> newEmphasis = prefix;
            newEmphasis.add(form);
            emphases.add(newEmphasis);
        }
    }
    return emphases;
}

/**
 * Generates all possible capitalizations of words in a given sentence, leaving non-word tokens unchanged.
 *
 * This function tokenizes the input sentence into individual words and non-word tokens, then generates
 * all possible variations where each word is either in uppercase or lowercase. Non-word tokens are kept as-is.
 *
 * @param sentence The input sentence as a single string.
 * @return A Set<string> containing all possible variations of the sentence with different capitalizations.
 */
Set<string> allEmphasesOf(const string& sentence) {
    // make sentence lowercase
    Vector<string> s = tokenize(sentence);
    Set<Vector<string>> emphases = allEmphasesOfRec(s, s.size() - 1);
    Set<string> emphasesStr;
    for(const auto& e: emphases) {
        string result;
        for(const string& word: e) {
            result += word;
        }
        emphasesStr.add(result);
    }
    return emphasesStr;
}

/* * * * * * Test Cases * * * * * */
#include "GUI/SimpleTest.h"

/* TODO: Add your own tests here. You know the drill - look for edge cases, think about
 * very small and very large cases, etc.
 */

STUDENT_TEST("Empty string emphases is empty set.") {
    // Define the expected result, which should be an empty set.
    Set<string> expected = {""};
    // Call the function with an empty string.
    Set<string> result = allEmphasesOf("");
    // Verify that the result matches the expected output.
    EXPECT_EQUAL(result, expected);
}

STUDENT_TEST("Emphases of sentence with mixed case and punctuation.") {
    // Define the input sentence
    string sentence = "Quoth the raven, \"Nevermore.\"";

    // Define the expected output set
    Set<string> expected = {
        "quoth the raven, \"nevermore.\"",
        "quoth the raven, \"NEVERMORE.\"",
        "quoth the RAVEN, \"nevermore.\"",
        "quoth the RAVEN, \"NEVERMORE.\"",
        "quoth THE raven, \"nevermore.\"",
        "quoth THE raven, \"NEVERMORE.\"",
        "quoth THE RAVEN, \"nevermore.\"",
        "quoth THE RAVEN, \"NEVERMORE.\"",
        "QUOTH the raven, \"nevermore.\"",
        "QUOTH the raven, \"NEVERMORE.\"",
        "QUOTH the RAVEN, \"nevermore.\"",
        "QUOTH the RAVEN, \"NEVERMORE.\"",
        "QUOTH THE raven, \"nevermore.\"",
        "QUOTH THE raven, \"NEVERMORE.\"",
        "QUOTH THE RAVEN, \"nevermore.\"",
        "QUOTH THE RAVEN, \"NEVERMORE.\""
    };

    // Call the function with the input sentence
    Set<string> result = allEmphasesOf(sentence);

    // Verify that the result matches the expected output
    EXPECT_EQUAL(result, expected);
}









/* * * * * * Test cases from the starter files below this point. * * * * * */

PROVIDED_TEST("Enumerates all options in a simple case.") {
    Set<string> expected = {
        "hello",
        "HELLO",
    };

    EXPECT_EQUAL(allEmphasesOf("Hello"), expected);
}

PROVIDED_TEST("Each option has the right length.") {
    string sentence = "Hello, world!";
    for (string option: allEmphasesOf(sentence)) {
        EXPECT_EQUAL(option.length(), sentence.length());
    }
}

PROVIDED_TEST("Enumerates all options in a more typical case.") {
    Set<string> expected = {
        "you are?",
        "you ARE?",
        "YOU are?",
        "YOU ARE?"
    };

    EXPECT_EQUAL(allEmphasesOf("You Are?"), expected);
}

PROVIDED_TEST("Stress Test: Recursion only branches on words (should take under a second)") {
    /* We're producing a string consisting of fifty copies of the * character. This tokenizes
     * into fifty separate stars. A star is the same whether or not you capitalize it - there
     * is no such thing as an "upper-case" or "lower-case" star. Therefore, your code should
     * not try to form two versions of the sentence, one with the star capitalized and one
     * without, because the two versions will end up being the same and the work to compute
     * both options will dramatically increase the runtime.
     *
     * For reference, if you do try branching the recursion and checking what happens both if
     * you capitalize the star and if you don't, you'll try exploring 2^50 different possible
     * capitalizations. That's 1,125,899,906,842,624 options, and even doing a billion of
     * these a second is going to take over two years to generate them all! And of course,
     * that's all wasted work, since there's only one way to capitalize this sentence, and
     * that's just to leave it as-is.
     *
     * If your code is hanging when this test is running, it likely means that your code is
     * trying to enumerate all of these options. See if you can edit your code so that, if
     * you're given a non-word token, you just leave it as-is and don't change anything.
     */
    string punctuation(50, '*'); // 50 copies of *

    /* The only emphasis is itself. */
    Set<string> expected = {
        punctuation
    };

    EXPECT_EQUAL(allEmphasesOf(punctuation), expected);
}

PROVIDED_TEST("Stress test: Generates each option once (should take at most a few seconds)") {
    /* This sentence has 13 words in it. There are therefore 2^13 = 8192 possible emphases for
     * the words there, which is a big number but not so large that the computer can't handle
     * it if generates each emphasis exactly once.
     *
     * On the other hand, if your code tries to generate the same emphases multiple times,
     * this test may take a very, very long time to complete, and might even appear to freeze
     * up.
     *
     * If your code gets stuck in this test, trace through your code and confirm that you can't
     * produce the same emphasis multiple times. Check to make sure you aren't, for example,
     * looping over every token in the input sentence and deciding which one to capitalize
     * next. The recursion here follows more of an include/exclude type pattern (more like
     * subsets and combinations) than it does a "which is next?" type pattern (more like
     * permutations).
     */
    string yeats = "Turing and turning in a widening gyre / the falcon cannot hear the falconer.";
    EXPECT_EQUAL(allEmphasesOf(yeats).size(), 8192);
}
