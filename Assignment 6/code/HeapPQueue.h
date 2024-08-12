#pragma once

#include "Demos/DataPoint.h"
#include "Demos/Utility.h"
#include "GUI/SimpleTest.h"

/**
 * Priority queue type implemented using a binary heap. Refer back to the assignment handout
 * for details about how binary heaps work.
 *
 * As a reminder, you are required to do all your own memory management using new[] and
 * delete[].
 */
class HeapPQueue {
public:
    /**
     * Creates a new, empty priority queue.
     */
    HeapPQueue();

    /**
     * Cleans up all memory allocated by this priorty queue. Remember, you're responsible
     * for managing your own memory!
     */
    ~HeapPQueue();

    /**
     * Adds a new data point into the queue. This operation runs in time O(log n),
     * where n is the number of elements in the queue.
     *
     * @param data The data point to add.
     */
    void enqueue(const DataPoint& data);

    /**
     * Removes and returns the lowest-weight data point in the priority queue. If multiple
     * elements are tied for having the loweset weight, any one of them may be returned.
     *
     * If the priority queue is empty, this function calls error() to report an error.
     *
     * This operation must run in time O(log n), where n is the number of elements in the
     * queue.
     *
     * @return The lowest-weight data point in the queue.
     */
    DataPoint dequeue();

    /**
     * Returns, but does not remove, the element that would next be removed via a call to
     * dequeue.
     *
     * If the priority queue is empty, this function calls error() to report an error.
     *
     * This operation must run in time O(1).
     *
     * @return
     */
    DataPoint peek() const;

    /**
     * Returns whether the priority queue is empty.
     *
     * This operation must run in time O(1).
     *
     * @return Whether the priority queue is empty.
     */
    bool isEmpty() const;

    /**
     * Returns the number of data points in this priority queue.
     *
     * This operation must run in time O(1).
     *
     * @return The number of elements in the priority queue.
     */
    int  size() const;

    /* This function exists purely for testing purposes. You can have it do whatever you'd
     * like and we won't be invoking it when grading. In the past, students have had this
     * function print out the array representing the heap, or information about how much
     * space is allocated, etc. Feel free to use it as you see fit!
     */
    void printDebugInfo();

private:
    static constexpr int INITIAL_CAPACITY = 100;
    int currentSize;
    int capacity;
    DataPoint* heap;

    // Factors for resizing
    double growFactor;
    double shrinkThreshold;

    /**
     * Helper function to restore the heap property by "bubbling up"
     * the element at the given index.
     *
     * @param index The index of the element to bubble up.
     */
    void bubbleUp(int index);

    /**
     * Helper function to restore the heap property by "bubbling down"
     * the element at the given index.
     *
     * @param index The index of the element to bubble up.
     */
    void bubbleDown(int index);

    /**
     * Returns the index of the parent of the element at the given index.
     *
     * @param index The index of the child element.
     * @return The index of the parent element.
     */
    int parent(int index) const;

    /**
     * Returns the index of the left child of the element at the given index.
     *
     * @param index The index of the parent element.
     * @return The index of the left child element.
     */
    int leftChild(int index) const;

    /**
     * Returns the index of the right child of the element at the given index.
     *
     * @param index The index of the parent element.
     * @return The index of the right child element.
     */
    int rightChild(int index) const;


    /**
     * Resizes the heap's internal array to a new capacity.
     *
     * This method allocates a new array of the specified capacity and copies the existing
     * elements from the old array to the new one. It then updates the heap to use the new
     * array and deallocates the old array. If the new capacity is smaller than the current
     * number of elements, an error is reported.
     *
     * The time complexity of this method is O(n), where n is the number of elements in the heap,
     * due to the copying of elements from the old array to the new one.
     *
     * @param newCapacity The desired capacity for the heap's internal array. This must be
     *                    greater than or equal to the current number of elements in the heap.
     *                    If it's less than the current size, an error is reported.
     */
    void resize(int newCapacity);


    /* By default, C++ will let you copy objects. The problem is that the default copy
     * just does an element-by-element copy, which with pointers will give invalid results.
     * This macro disables copying of this type. For more details about how this works, and
     * for more information about how to override the default behavior, take CS106L!
     */
    DISALLOW_COPYING_OF(HeapPQueue);

    /* Grants STUDENT_TEST and PROVIDED_TEST access to the private section of this class.
     * This allows tests to check private fields to make sure they have the right values
     * and to test specific helper functions.
     */
    ALLOW_TEST_ACCESS();
};

