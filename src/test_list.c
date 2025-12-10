/**
 * Test file for the LIST data structure in hash_table.h
 * 
 * This file tests the following operations:
 * - create_owned_list()
 * - add_element()
 * - find_key()
 * - contains_element()
 * - delete_element()
 * - free_head()
 * - free_list()
 * - free_owned_list()
 */

#include <stdio.h>
#include "hash_table.h"

// Test counters
static int tests_passed = 0;
static int tests_failed = 0;

// Helper macro for test assertions
#define TEST_ASSERT(condition, test_name) do { \
    if (condition) { \
        printf("[PASS] %s\n", test_name); \
        tests_passed++; \
    } else { \
        printf("[FAIL] %s\n", test_name); \
        tests_failed++; \
    } \
} while (0)

// Helper function to count elements in a list
static int count_elements(LIST list) {
    int count = 0;
    while (*list) {
        count++;
        list = &(*list)->next;
    }
    return count;
}

// ============================================================================
// Test: create_owned_list
// ============================================================================
void test_create_owned_list() {
    printf("\n--- Testing create_owned_list ---\n");
    
    LIST list = create_owned_list();
    
    TEST_ASSERT(list != NULL, "create_owned_list returns non-NULL pointer");
    TEST_ASSERT(*list == NULL, "newly created list is empty (head is NULL)");
    
    free_owned_list(list);
}

// ============================================================================
// Test: add_element
// ============================================================================
void test_add_element() {
    printf("\n--- Testing add_element ---\n");
    
    LIST list = create_owned_list();
    
    // Test adding single element
    add_element(list, 42);
    TEST_ASSERT(*list != NULL, "list is not empty after adding element");
    TEST_ASSERT((*list)->key == 42, "first element has correct key (42)");
    TEST_ASSERT((*list)->next == NULL, "single element has NULL next pointer");
    TEST_ASSERT(count_elements(list) == 1, "list has exactly 1 element");
    
    // Test adding second element (should be at head)
    add_element(list, 100);
    TEST_ASSERT((*list)->key == 100, "new element becomes head (100)");
    TEST_ASSERT((*list)->next != NULL, "head has non-NULL next pointer");
    TEST_ASSERT((*list)->next->key == 42, "second element is previous head (42)");
    TEST_ASSERT(count_elements(list) == 2, "list has exactly 2 elements");
    
    // Test adding third element
    add_element(list, 7);
    TEST_ASSERT((*list)->key == 7, "newest element is head (7)");
    TEST_ASSERT(count_elements(list) == 3, "list has exactly 3 elements");
    
    free_owned_list(list);
}

// ============================================================================
// Test: find_key
// ============================================================================
void test_find_key() {
    printf("\n--- Testing find_key ---\n");
    
    LIST list = create_owned_list();
    
    // Test finding in empty list
    LIST result = find_key(list, 42);
    TEST_ASSERT(result == NULL, "find_key returns NULL for empty list");
    
    // Add elements
    add_element(list, 10);
    add_element(list, 20);
    add_element(list, 30);
    
    // Test finding existing elements
    result = find_key(list, 30);
    TEST_ASSERT(result != NULL, "find_key returns non-NULL for head element (30)");
    TEST_ASSERT((*result)->key == 30, "found element has correct key (30)");
    
    result = find_key(list, 20);
    TEST_ASSERT(result != NULL, "find_key returns non-NULL for middle element (20)");
    TEST_ASSERT((*result)->key == 20, "found element has correct key (20)");
    
    result = find_key(list, 10);
    TEST_ASSERT(result != NULL, "find_key returns non-NULL for tail element (10)");
    TEST_ASSERT((*result)->key == 10, "found element has correct key (10)");
    
    // Test finding non-existent element
    result = find_key(list, 999);
    TEST_ASSERT(result == NULL, "find_key returns NULL for non-existent key (999)");
    
    free_owned_list(list);
}

// ============================================================================
// Test: contains_element
// ============================================================================
void test_contains_element() {
    printf("\n--- Testing contains_element ---\n");
    
    LIST list = create_owned_list();
    
    // Test empty list
    TEST_ASSERT(!contains_element(list, 42), "contains_element returns false for empty list");
    
    // Add elements
    add_element(list, 5);
    add_element(list, 15);
    add_element(list, 25);
    
    // Test existing elements
    TEST_ASSERT(contains_element(list, 5), "contains_element returns true for existing key (5)");
    TEST_ASSERT(contains_element(list, 15), "contains_element returns true for existing key (15)");
    TEST_ASSERT(contains_element(list, 25), "contains_element returns true for existing key (25)");
    
    // Test non-existing elements
    TEST_ASSERT(!contains_element(list, 0), "contains_element returns false for non-existent key (0)");
    TEST_ASSERT(!contains_element(list, 10), "contains_element returns false for non-existent key (10)");
    TEST_ASSERT(!contains_element(list, 100), "contains_element returns false for non-existent key (100)");
    
    free_owned_list(list);
}

// ============================================================================
// Test: delete_element
// ============================================================================
void test_delete_element() {
    printf("\n--- Testing delete_element ---\n");
    
    LIST list = create_owned_list();
    
    // Test deleting from empty list (should not crash)
    delete_element(list, 42);
    TEST_ASSERT(*list == NULL, "delete_element on empty list keeps it empty");
    
    // Add elements
    add_element(list, 1);
    add_element(list, 2);
    add_element(list, 3);
    add_element(list, 4);
    add_element(list, 5);
    // List is now: 5 -> 4 -> 3 -> 2 -> 1
    
    TEST_ASSERT(count_elements(list) == 5, "list has 5 elements before deletion");
    
    // Test deleting head element
    delete_element(list, 5);
    TEST_ASSERT(!contains_element(list, 5), "deleted element (5) no longer in list");
    TEST_ASSERT(count_elements(list) == 4, "list has 4 elements after deleting head");
    TEST_ASSERT((*list)->key == 4, "new head is 4 after deleting 5");
    
    // Test deleting middle element
    delete_element(list, 3);
    TEST_ASSERT(!contains_element(list, 3), "deleted element (3) no longer in list");
    TEST_ASSERT(count_elements(list) == 3, "list has 3 elements after deleting middle");
    // List is now: 4 -> 2 -> 1
    
    // Test deleting tail element
    delete_element(list, 1);
    TEST_ASSERT(!contains_element(list, 1), "deleted element (1) no longer in list");
    TEST_ASSERT(count_elements(list) == 2, "list has 2 elements after deleting tail");
    // List is now: 4 -> 2
    
    // Test deleting non-existent element (should not crash or modify list)
    delete_element(list, 999);
    TEST_ASSERT(count_elements(list) == 2, "list unchanged after deleting non-existent key");
    
    // Verify remaining elements
    TEST_ASSERT(contains_element(list, 4), "element 4 still in list");
    TEST_ASSERT(contains_element(list, 2), "element 2 still in list");
    
    free_owned_list(list);
}

// ============================================================================
// Test: free_head
// ============================================================================
void test_free_head() {
    printf("\n--- Testing free_head ---\n");
    
    LIST list = create_owned_list();
    
    add_element(list, 100);
    add_element(list, 200);
    add_element(list, 300);
    // List is now: 300 -> 200 -> 100
    
    TEST_ASSERT(count_elements(list) == 3, "list has 3 elements before free_head");
    TEST_ASSERT((*list)->key == 300, "head is 300 before free_head");
    
    free_head(list);
    TEST_ASSERT(count_elements(list) == 2, "list has 2 elements after first free_head");
    TEST_ASSERT((*list)->key == 200, "head is 200 after first free_head");
    
    free_head(list);
    TEST_ASSERT(count_elements(list) == 1, "list has 1 element after second free_head");
    TEST_ASSERT((*list)->key == 100, "head is 100 after second free_head");
    
    free_head(list);
    TEST_ASSERT(count_elements(list) == 0, "list is empty after third free_head");
    TEST_ASSERT(*list == NULL, "list head is NULL after freeing all elements");
    
    free_owned_list(list);
}

// ============================================================================
// Test: Edge cases with DEFAULT_KEY (0)
// ============================================================================
void test_default_key_handling() {
    printf("\n--- Testing DEFAULT_KEY (0) handling ---\n");
    
    LIST list = create_owned_list();
    
    // Add DEFAULT_KEY (0) to the list
    add_element(list, DEFAULT_KEY);
    TEST_ASSERT(contains_element(list, DEFAULT_KEY), "DEFAULT_KEY (0) can be added and found");
    TEST_ASSERT(count_elements(list) == 1, "list has 1 element after adding DEFAULT_KEY");
    
    // Add more elements
    add_element(list, 1);
    add_element(list, 2);
    
    TEST_ASSERT(contains_element(list, DEFAULT_KEY), "DEFAULT_KEY still findable with other elements");
    
    // Delete DEFAULT_KEY
    delete_element(list, DEFAULT_KEY);
    TEST_ASSERT(!contains_element(list, DEFAULT_KEY), "DEFAULT_KEY deleted successfully");
    TEST_ASSERT(count_elements(list) == 2, "list has 2 elements after deleting DEFAULT_KEY");
    
    free_owned_list(list);
}

// ============================================================================
// Test: Large number of elements
// ============================================================================
void test_many_elements() {
    printf("\n--- Testing with many elements ---\n");
    
    LIST list = create_owned_list();
    const int NUM_ELEMENTS = 1000;
    
    // Add many elements
    for (int i = 1; i <= NUM_ELEMENTS; i++) {
        add_element(list, (unsigned int)i);
    }
    
    TEST_ASSERT(count_elements(list) == NUM_ELEMENTS, "list has correct count after adding 1000 elements");
    
    // Verify first and last added elements
    TEST_ASSERT(contains_element(list, 1), "first added element (1) is present");
    TEST_ASSERT(contains_element(list, NUM_ELEMENTS), "last added element (1000) is present");
    TEST_ASSERT(contains_element(list, NUM_ELEMENTS / 2), "middle element (500) is present");
    
    // Delete some elements
    delete_element(list, 1);
    delete_element(list, NUM_ELEMENTS);
    delete_element(list, NUM_ELEMENTS / 2);
    
    TEST_ASSERT(!contains_element(list, 1), "element 1 deleted");
    TEST_ASSERT(!contains_element(list, NUM_ELEMENTS), "element 1000 deleted");
    TEST_ASSERT(!contains_element(list, NUM_ELEMENTS / 2), "element 500 deleted");
    TEST_ASSERT(count_elements(list) == NUM_ELEMENTS - 3, "list has correct count after deletions");
    
    free_owned_list(list);
}

// ============================================================================
// Test: Duplicate keys
// ============================================================================
void test_duplicate_keys() {
    printf("\n--- Testing duplicate keys ---\n");
    
    LIST list = create_owned_list();
    
    // Add duplicate keys
    add_element(list, 42);
    add_element(list, 42);
    add_element(list, 42);
    
    TEST_ASSERT(count_elements(list) == 3, "list allows duplicate keys (3 elements with same key)");
    TEST_ASSERT(contains_element(list, 42), "contains_element returns true for duplicate key");
    
    // Delete should only remove the first occurrence
    delete_element(list, 42);
    TEST_ASSERT(count_elements(list) == 2, "delete_element removes only one occurrence (2 remaining)");
    TEST_ASSERT(contains_element(list, 42), "other duplicates still exist");
    
    delete_element(list, 42);
    delete_element(list, 42);
    TEST_ASSERT(count_elements(list) == 0, "all duplicates deleted");
    TEST_ASSERT(!contains_element(list, 42), "no more duplicates in list");
    
    free_owned_list(list);
}

// ============================================================================
// Main test runner
// ============================================================================
int main() {
    printf("===============================================\n");
    printf("    LIST Data Structure Test Suite\n");
    printf("===============================================\n");
    
    test_create_owned_list();
    test_add_element();
    test_find_key();
    test_contains_element();
    test_delete_element();
    test_free_head();
    test_default_key_handling();
    test_many_elements();
    test_duplicate_keys();
    
    printf("\n===============================================\n");
    printf("    Test Results Summary\n");
    printf("===============================================\n");
    printf("Tests passed: %d\n", tests_passed);
    printf("Tests failed: %d\n", tests_failed);
    printf("Total tests:  %d\n", tests_passed + tests_failed);
    printf("===============================================\n");
    
    if (tests_failed > 0) {
        printf("\nSome tests FAILED!\n");
        return 1;
    } else {
        printf("\nAll tests PASSED!\n");
        return 0;
    }
}
