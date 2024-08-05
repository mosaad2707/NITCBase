#include "Buffer/StaticBuffer.h"
#include "Cache/OpenRelTable.h"
#include "Disk_Class/Disk.h"
#include "FrontendInterface/FrontendInterface.h"
#include <iostream>

// Stage 1

// Function to write "hello" into a disk block and then read it back
void printhello() {
    Disk disk_run;  // Initialize a Disk object for reading and writing blocks

    unsigned char buffer[BLOCK_SIZE];  // Create a buffer to hold block data

    // Read block number 7000 into the buffer
    Disk::readBlock(buffer, 7000);

    // Define a message to write to the block
    char message[] = "hello";

    // Copy the message "hello" into the buffer at offset 20
    memcpy(buffer + 20, message, 6);

    // Write the modified buffer back to block number 7000
    Disk::writeBlock(buffer, 7000);

    unsigned char buffer2[BLOCK_SIZE];  // Create another buffer to read back the block

    char message2[6];  // Buffer to hold the read message

    // Read block number 7000 again into buffer2
    Disk::readBlock(buffer2, 7000);

    // Copy the message from buffer2 at offset 20 into message2
    memcpy(message2, buffer2 + 20, 6);

    // Print the message that was read back
    std::cout << message2;
}

// Function to print the content of a specified disk block
void printt_block_content(int block_num) {
    Disk disk_run;  // Initialize a Disk object

    unsigned char buffer[BLOCK_SIZE];  // Create a buffer to hold block data

    // Read the specified block number into the buffer
    Disk::readBlock(buffer, block_num);

    // Print the block number and its content byte by byte
    std::cout << "Block " << block_num << ": ";
    for (int j = 0; j < BLOCK_SIZE; j++) {
        std::cout << (int)buffer[j] << " ";  // Print each byte as an integer
    }
    std::cout << std::endl;  // End the line after printing the block content
}

// Stage 2

// Function to print relations and their attributes from catalogs
void print_relations_attributes() {
    RecBuffer relCatBuffer(RELCAT_BLOCK);  // Initialize a RecBuffer for the relation catalog block
    RecBuffer attrCatBuffer(ATTRCAT_BLOCK);  // Initialize a RecBuffer for the attribute catalog block

    HeadInfo relCatHeader;  // Declare a HeadInfo structure for the relation catalog header
    HeadInfo attrCatHeader;  // Declare a HeadInfo structure for the attribute catalog header

    // Load the headers of both blocks into relCatHeader and attrCatHeader
    relCatBuffer.getHeader(&relCatHeader);
    attrCatBuffer.getHeader(&attrCatHeader);

    // Iterate over each entry in the relation catalog
    for (int i = 0; i < relCatHeader.numEntries; i++) {
        Attribute relCatRecord[RELCAT_NO_ATTRS];  // Declare an array to store the relation catalog record

        // Load the record at index i into relCatRecord
        relCatBuffer.getRecord(relCatRecord, i);

        // Print the name of the relation from the catalog
        printf("Relation: %s\n", relCatRecord[RELCAT_REL_NAME_INDEX].sVal);

        // Initialize the attribute catalog block number to start reading
        int attrCatBlockNumber = ATTRCAT_BLOCK;

        // Loop to read through attribute catalog blocks linked by right block pointers
        while (attrCatBlockNumber != -1) {
            RecBuffer attrCatBuffer(attrCatBlockNumber);  // Initialize a RecBuffer for the current attribute catalog block
            HeadInfo attrCatHeader;  // Declare a HeadInfo structure for the attribute catalog header
            attrCatBuffer.getHeader(&attrCatHeader);  // Load the header into attrCatHeader
            attrCatBlockNumber = attrCatHeader.rblock;  // Update the block number to the next block

            // Iterate over each entry in the attribute catalog
            for (int j = 0; j < attrCatHeader.numEntries; j++) {
                Attribute attrCatRecord[ATTRCAT_NO_ATTRS];  // Declare an array to store the attribute catalog record

                // Load the record at index j into attrCatRecord
                attrCatBuffer.getRecord(attrCatRecord, j);

                // Check if the attribute belongs to the current relation
                if (strcmp(attrCatRecord[ATTRCAT_REL_NAME_INDEX].sVal, relCatRecord[RELCAT_REL_NAME_INDEX].sVal))
                    continue;  // If it doesn't match, skip to the next attribute

                // Determine the type of the attribute (NUM or STR)
                const char* attrType = (attrCatRecord[ATTRCAT_ATTR_TYPE_INDEX].nVal == NUMBER)
                    ? "NUM"
                    : "STR";

                // Print the attribute name and type
                printf("%s: %s\n", attrCatRecord[ATTRCAT_ATTR_NAME_INDEX].sVal, attrType);
            }
        }
        printf("\n");  // Print a newline to separate different relations
    }
}

// Main function
int main(int argc, char* argv[]) {
    Disk disk_run;  // Initialize a Disk object
    print_relations_attributes();  // Call the function to print relations and their attributes
    return 0;  // Return success
}
