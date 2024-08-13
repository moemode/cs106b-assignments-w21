#include "HeapPQueue.h"
#include "vector.h"
#include <algorithm>
using namespace std;

HeapPQueue::HeapPQueue()
    : currentSize(0),
    capacity(INITIAL_CAPACITY),
    heap(new DataPoint[INITIAL_CAPACITY]),
    growFactor(2.0),
    shrinkThreshold(0.25)  {
}


HeapPQueue::~HeapPQueue() {
    delete[] heap;
    heap = nullptr;
}

int HeapPQueue::parent(int index) const {
    return (index - 1) / 2;
}


int HeapPQueue::leftChild(int index) const {
    return 2 * index + 1;
}


int HeapPQueue::rightChild(int index) const {
    return 2 * index + 2;
}

void HeapPQueue::bubbleUp(int index) {
    while (index > 0 && heap[index].weight < heap[parent(index)].weight) {
        swap(heap[index], heap[parent(index)]);
        index = parent(index);
    }
}

void HeapPQueue::enqueue(const DataPoint& data) {
    if (size() == capacity) {
        resize(static_cast<int>(capacity * growFactor));
    }
    heap[currentSize] = data;
    currentSize += 1;
    bubbleUp(currentSize - 1);
}

void HeapPQueue::resize(int newCapacity) {
    if (newCapacity < currentSize) {
        error("New capacity must be greater than or equal to the current size.");
    }
    DataPoint* newHeap = new DataPoint[newCapacity];
    for (int i = 0; i < currentSize; ++i) {
        newHeap[i] = heap[i];
    }
    delete[] heap;
    heap = newHeap;
    capacity = newCapacity;
}



int HeapPQueue::size() const {
    return currentSize;
}

DataPoint HeapPQueue::peek() const {
    if (isEmpty()) {
        error("Must not peek from an empty HeapPQueue");
    }
    return heap[0];
}

void HeapPQueue::bubbleDown(int index) {
    int size = currentSize;  // Get the current size of the heap
    while (true) {
        int left = leftChild(index);
        int right = rightChild(index);
        int smallest = index;
        if (left < size && heap[left].weight < heap[smallest].weight) {
            smallest = left;
        }
        if (right < size && heap[right].weight < heap[smallest].weight) {
            smallest = right;
        }
        // If the smallest element is not the current element, swap and continue
        if (smallest != index) {
            std::swap(heap[index], heap[smallest]);
            index = smallest;  // Repeat one level deeper
        } else {
            break;  // The heap property is satisfied, exit the loop
        }
    }
}

DataPoint HeapPQueue::dequeue() {
    if (isEmpty()) {
        error("Must not dequeue from an empty HeapPQueue");
    }
    DataPoint root = heap[0];
    heap[0] = heap[currentSize - 1];
    currentSize--;
    bubbleDown(0);
    if (size() < shrinkThreshold * capacity) {
        resize(max(INITIAL_CAPACITY, static_cast<int>(capacity / growFactor))); // Use the reciprocal of growFactor
    }
    return root;
}

bool HeapPQueue::isEmpty() const {
    return currentSize == 0;
}

/* This function prints the contents of the heap in a tree-like structure,
 * which is useful for debugging. The heap is displayed level by level,
 * showing how elements are organized within the underlying array structure.
 *
 * The function iterates through the heap and prints each element. When it
 * encounters the start of a new level in the binary tree (determined by the
 * index), it moves to a new line to reflect the tree structure visually.
 *
 * This function is intended for debugging purposes to help visualize the
 * internal state of the heap.
 */
void HeapPQueue::printDebugInfo() {
    int level = 0; // Track the current level in the heap

    std::cout << "Heap contents (tree view):" << std::endl;

    for (int i = 0; i < currentSize; i++) {
        // Check if we're at the start of a new level
        if (i == (1 << level) - 1) { // 2^level - 1 indicates the start of a new level
            if (level > 0) {
                std::cout << std::endl; // Move to the next line for a new level
            }
            level++;
        }

        std::cout << heap[i] << " "; // Print the current element
    }

    std::cout << std::endl; // Final newline for clean output
}


/* * * * * * Test Cases Below This Point * * * * * */


STUDENT_TEST("Heap handles alternating enqueues and dequeues with resizing") {
    HeapPQueue pq;
    // Enqueue elements to fill the heap and trigger resizing
    for (int i = 0; i < 120; i++) {
        pq.enqueue({ "elem" + to_string(i), i });
    }
    // Dequeue elements to reduce the size and potentially trigger resizing down
    for (int i = 0; i < 100; i++) {
        pq.dequeue();
    }
    // Continue alternating enqueues and dequeues
    for (int i = 120; i < 150; i++) {
        pq.enqueue({ "elem" + to_string(i), i });
    }

    // Check the remaining elements
    Vector<DataPoint> remainingElements;
    while (!pq.isEmpty()) {
        remainingElements.add(pq.dequeue());
    }
    // Expected elements are from 120 to 149, with the size of 20 elements
    Vector<DataPoint> expectedElements;
    for (int i = 100; i < 150; i++) {
        expectedElements.add({ "elem" + to_string(i), i });
    }
    // The heap should contain the expected elements
    EXPECT_EQUAL(remainingElements.size(), expectedElements.size());
    for (int i = 0; i < expectedElements.size(); i++) {
        EXPECT_EQUAL(remainingElements[i], expectedElements[i]);
    }
    // The heap should be empty now
    EXPECT_EQUAL(pq.size(), 0);
    EXPECT_EQUAL(pq.isEmpty(), true);
}

/* * * * * Provided Tests Below This Point * * * * */

PROVIDED_TEST("Newly-created heap is empty.") {
    HeapPQueue pq;

    EXPECT(pq.isEmpty());
    EXPECT(pq.size() == 0);
}

PROVIDED_TEST("Enqueue / dequeue single element") {
    HeapPQueue pq;
    DataPoint point = { "enqueue me!", 4 };
    pq.enqueue(point);
    EXPECT_EQUAL(pq.size(), 1);
    EXPECT_EQUAL(pq.isEmpty(), false);

    EXPECT_EQUAL(pq.dequeue(), point);
    EXPECT_EQUAL(pq.size(), 0);
    EXPECT_EQUAL(pq.isEmpty(), true);

    pq.enqueue(point);
    EXPECT_EQUAL(pq.size(), 1);
    EXPECT_EQUAL(pq.isEmpty(), false);

    EXPECT_EQUAL(pq.dequeue(), point);
    EXPECT_EQUAL(pq.size(), 0);
    EXPECT_EQUAL(pq.isEmpty(), true);
}

PROVIDED_TEST("Dequeue / peek on empty heap throws error") {
    HeapPQueue pq;

    EXPECT(pq.isEmpty());
    EXPECT_ERROR(pq.dequeue());
    EXPECT_ERROR(pq.peek());
}

PROVIDED_TEST("Enqueue elements in sorted order.") {
    HeapPQueue pq;
    for (int i = 0; i < 10; i++) {
        pq.enqueue({ "elem" + to_string(i), i });
    }

    EXPECT_EQUAL(pq.size(), 10);
    for (int i = 0; i < 10; i++) {
        EXPECT_EQUAL(pq.peek(), { "elem" + to_string(i), i });
        EXPECT_EQUAL(pq.dequeue(), { "elem" + to_string(i), i });
    }
    EXPECT_EQUAL(pq.size(), 0);
    EXPECT_EQUAL(pq.isEmpty(), true);
}

PROVIDED_TEST("Enqueue many elements in sorted order.") {
    HeapPQueue pq;
    for (int i = 0; i < 10000; i++) {
        pq.enqueue({ "elem" + to_string(i), i });
    }

    EXPECT_EQUAL(pq.size(), 10000);
    for (int i = 0; i < 10000; i++) {
        EXPECT_EQUAL(pq.peek(), { "elem" + to_string(i), i });
        EXPECT_EQUAL(pq.dequeue(), { "elem" + to_string(i), i });
    }
    EXPECT_EQUAL(pq.size(), 0);
    EXPECT_EQUAL(pq.isEmpty(), true);
}

PROVIDED_TEST("Enqueue elements in reverse-sorted order.") {
    HeapPQueue pq;
    for (int i = 10; i >= 0; i--) {
        pq.enqueue({ "elem" + to_string(i), i });
    }

    EXPECT_EQUAL(pq.size(), 11);
    for (int i = 0; i <= 10; i++) {
        EXPECT_EQUAL(pq.peek(), { "elem" + to_string(i), i });
        EXPECT_EQUAL(pq.dequeue(), { "elem" + to_string(i), i });
    }
    EXPECT_EQUAL(pq.size(), 0);
    EXPECT_EQUAL(pq.isEmpty(), true);
}

PROVIDED_TEST("Enqueue many elements in reverse-sorted order.") {
    HeapPQueue pq;
    for (int i = 10000; i >= 0; i--) {
        pq.enqueue({ "elem" + to_string(i), i });
    }

    EXPECT_EQUAL(pq.size(), 10001);
    for (int i = 0; i <= 10000; i++) {
        auto removed = pq.dequeue();
        DataPoint expected = {
            "elem" + to_string(i), i
        };
        EXPECT_EQUAL(removed, expected);
    }
    EXPECT_EQUAL(pq.size(), 0);
    EXPECT_EQUAL(pq.isEmpty(), true);
}

PROVIDED_TEST("Insert ascending and descending sequences.") {
    HeapPQueue pq;
    for (int i = 0; i < 20; i++) {
        pq.enqueue({ "a" + to_string(i), 2 * i });
    }
    for (int i = 19; i >= 0; i--) {
        pq.enqueue({ "b" + to_string(i), 2 * i + 1 });
    }

    EXPECT_EQUAL(pq.size(), 40);
    for (int i = 0; i < 40; i++) {
        auto removed = pq.dequeue();
        EXPECT_EQUAL(removed.weight, i);
    }
    EXPECT_EQUAL(pq.size(), 0);
    EXPECT_EQUAL(pq.isEmpty(), true);
}

PROVIDED_TEST("Insert large ascending and descending sequences.") {
    HeapPQueue pq;
    for (int i = 0; i < 20000; i++) {
        pq.enqueue({ "a" + to_string(i), 2 * i });
    }
    for (int i = 19999; i >= 0; i--) {
        pq.enqueue({ "b" + to_string(i), 2 * i + 1 });
    }

    EXPECT_EQUAL(pq.size(), 40000);
    for (int i = 0; i < 40000; i++) {
        auto removed = pq.dequeue();
        EXPECT_EQUAL(removed.weight, i);
    }
    EXPECT_EQUAL(pq.size(), 0);
    EXPECT_EQUAL(pq.isEmpty(), true);
}

PROVIDED_TEST("Insert random permutation.") {
    Vector<DataPoint> sequence = {
        { "A", 0 },
        { "D", 3 },
        { "F", 5 },
        { "G", 6 },
        { "C", 2 },
        { "H", 7 },
        { "I", 8 },
        { "B", 1 },
        { "E", 4 },
        { "J", 9 },
    };

    HeapPQueue pq;
    for (DataPoint elem: sequence) {
        pq.enqueue(elem);
    }

    EXPECT_EQUAL(pq.size(), sequence.size());

    for (int i = 0; i < 10; i++) {
        auto removed = pq.dequeue();
        DataPoint expected = {
            string(1, 'A' + i), i
        };
        EXPECT_EQUAL(removed, expected);
    }
    EXPECT_EQUAL(pq.size(), 0);
    EXPECT_EQUAL(pq.isEmpty(), true);
}

PROVIDED_TEST("Insert duplicate elements.") {
    HeapPQueue pq;
    for (int i = 0; i < 20; i++) {
        pq.enqueue({ "a" + to_string(i), i });
    }
    for (int i = 19; i >= 0; i--) {
        pq.enqueue({ "b" + to_string(i), i });
    }

    EXPECT_EQUAL(pq.size(), 40);
    for (int i = 0; i < 20; i++) {
        auto one = pq.dequeue();
        auto two = pq.dequeue();

        EXPECT_EQUAL(one.weight, i);
        EXPECT_EQUAL(two.weight, i);
    }
    EXPECT_EQUAL(pq.size(), 0);
    EXPECT_EQUAL(pq.isEmpty(), true);
}

PROVIDED_TEST("Insert many duplicate elements.") {
    HeapPQueue pq;
    for (int i = 0; i < 20000; i++) {
        pq.enqueue({ "a" + to_string(i), i });
    }
    for (int i = 19999; i >= 0; i--) {
        pq.enqueue({ "b" + to_string(i), i });
    }

    EXPECT_EQUAL(pq.size(), 40000);
    for (int i = 0; i < 20000; i++) {
        auto one = pq.dequeue();
        auto two = pq.dequeue();

        EXPECT_EQUAL(one.weight, i);
        EXPECT_EQUAL(two.weight, i);
    }
    EXPECT_EQUAL(pq.size(), 0);
    EXPECT_EQUAL(pq.isEmpty(), true);
}

PROVIDED_TEST("Handles data points with empty string name.") {
    HeapPQueue pq;
    for (int i = 0; i < 10; i++) {
        pq.enqueue({ "" , i });
    }
    EXPECT_EQUAL(pq.size(), 10);
    for (int i = 0; i < 10; i++) {
        EXPECT_EQUAL(pq.dequeue(), { "", i });
    }
    EXPECT_EQUAL(pq.size(), 0);
    EXPECT(pq.isEmpty());
}

PROVIDED_TEST("Handles many data points with empty string name.") {
    HeapPQueue pq;
    for (int i = 0; i < 10000; i++) {
        pq.enqueue({ "" , i });
    }
    EXPECT_EQUAL(pq.size(), 10000);
    for (int i = 0; i < 10000; i++) {
        EXPECT_EQUAL(pq.dequeue(), { "", i });
    }
    EXPECT_EQUAL(pq.size(), 0);
    EXPECT(pq.isEmpty());
}

PROVIDED_TEST("Handles data points with negative weights.") {
    HeapPQueue pq;
    for (int i = -10; i < 10; i++) {
        pq.enqueue({ "" , i });
    }
    EXPECT_EQUAL(pq.size(), 20);
    for (int i = -10; i < 10; i++) {
        EXPECT_EQUAL(pq.dequeue().weight, i);
    }
}

PROVIDED_TEST("Handles many data points with negative weights.") {
    HeapPQueue pq;
    for (int i = -10000; i < 10000; i++) {
        pq.enqueue({ "" , i });
    }
    EXPECT_EQUAL(pq.size(), 20000);
    for (int i = -10000; i < 10000; i++) {
        EXPECT_EQUAL(pq.dequeue().weight, i);
    }
}

PROVIDED_TEST("Interleave enqueues and dequeues.") {
    HeapPQueue pq;
    int n = 100;
    for (int i = n / 2; i < n; i++) {
        pq.enqueue({"", i});
    }
    EXPECT_EQUAL(pq.size(), n / 2);
    for (int i = n / 2; i < n; i++) {
        EXPECT_EQUAL(pq.dequeue().weight, i);
    }
    EXPECT_EQUAL(pq.size(), 0);

    for (int i = 0; i < n / 2; i++) {
        pq.enqueue({"", i});
    }
    EXPECT_EQUAL(pq.size(), n / 2);
    for (int i = 0; i < n / 2; i++) {
        EXPECT_EQUAL(pq.dequeue().weight, i);
    }
    EXPECT_EQUAL(pq.size(), 0);
}

PROVIDED_TEST("Interleave many enqueues and dequeues.") {
    HeapPQueue pq;
    int n = 10000;
    for (int i = n / 2; i < n; i++) {
        pq.enqueue({"", i});
    }
    EXPECT_EQUAL(pq.size(), n / 2);
    for (int i = n / 2; i < n; i++) {
        EXPECT_EQUAL(pq.dequeue().weight, i);
    }
    EXPECT_EQUAL(pq.size(), 0);

    for (int i = 0; i < n / 2; i++) {
        pq.enqueue({"", i});
    }
    EXPECT_EQUAL(pq.size(), n / 2);
    for (int i = 0; i < n / 2; i++) {
        EXPECT_EQUAL(pq.dequeue().weight, i);
    }
    EXPECT_EQUAL(pq.size(), 0);
}

PROVIDED_TEST("Stress test: cycle 250,000 elems (should take at most a few seconds)") {
    HeapPQueue pq;
    int n = 250000;
    for (int i = 0; i < n; i++) {
        pq.enqueue({ "", randomInteger(0, 100000) });
    }
    EXPECT_EQUAL(pq.size(), n);

    for (int i = 0; i < n; i++) {
        pq.dequeue();
    }
    EXPECT_EQUAL(pq.size(), 0);
    EXPECT_EQUAL(pq.isEmpty(), true);

    for (int i = 0; i < n; i++) {
        pq.enqueue({ "", randomInteger(0, 100000) });
    }
    EXPECT_EQUAL(pq.size(), n);
}
