#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/bt.h"
#include "../include/huffman.h"

int main() {
    int choice;
    char input_path[256], output_path[256];
    
    printf("=== HUFFMAN CODING PROJECT ===\n");
    printf("Binary File Compression/Decompression\n\n");
    
    while (1) {
        printf("=== MAIN MENU ===\n");
        printf("1. Compress a file\n");
        printf("2. Decompress a file\n");
        printf("3. Exit program\n");
        printf("Enter your choice (1-3): ");
        
        if (scanf("%d", &choice) != 1) {
            printf("Invalid input! Please enter a number.\n");
            while (getchar() != '\n'); // Clear input buffer
            continue;
        }
        
        if (choice == 1) {
            // ========== COMPRESSION ==========
            printf("\n--- FILE COMPRESSION ---\n");
            printf("Enter the file to compress (e.g., test.txt): ");
            scanf("%255s", input_path);  // Correction : limiter la lecture à 255 caractères
            
            // CORRECTION : Vérifier si l'utilisateur a mis un espace
            if (strchr(input_path, ' ') != NULL) {
                printf("ERROR: File name contains spaces! Please use filename without spaces.\n");
                printf("You entered: '%s'\n", input_path);
                printf("Use: test.txt (not 'test .txt')\n");
                continue;
            }
            
            printf("Enter name for compressed file (e.g., compressed.huf): ");
            scanf("%255s", output_path);
            
            printf("\n[1] Opening file '%s'...\n", input_path);
            FILE *in = fopen(input_path, "rb");
            if (!in) {
                printf("ERROR: Cannot open file '%s'!\n", input_path);
                printf("Make sure:\n");
                printf("  1. The file exists\n");
                printf("  2. You're in the right directory\n");
                printf("  3. The filename is correct (no spaces at end)\n");
                continue;
            }
            
            // Read file - CORRECTION IMPORTANTE
            printf("[2] Reading file content...\n");
            int size;
            unsigned char *data = readFile(in, &size);
            fclose(in);
            
            // CORRECTION CRITIQUE : Vérifier d'abord si readFile a retourné NULL
            if (!data) {
                // Deux possibilités : fichier vide ou erreur mémoire
                if (size == 0) {
                    printf("ERROR: File '%s' is EMPTY (0 bytes)! Nothing to compress.\n", input_path);
                    printf("Note: A file created with 'echo \"\" > file.txt' is NOT empty (contains newline).\n");
                    printf("To create truly empty file:\n");
                    printf("  Windows: type nul > file.txt\n");
                    printf("  Linux/Mac: touch file.txt\n");
                } else {
                    printf("ERROR: Memory allocation failed while reading file!\n");
                }
                continue;  // Pas besoin de free(data) car data est NULL
            }
            
            printf("File size: %d bytes\n", size);
            
            // Count frequencies
            printf("[3] Calculating character frequencies...\n");
            int freq[256] = {0};
            countFrequencies(data, size, freq);
            
            // Check number of distinct characters
            int distinct_chars = 0;
            int most_frequent_char = 0;
            for (int i = 0; i < 256; i++) {
                if (freq[i] > 0) {
                    distinct_chars++;
                    most_frequent_char = i;
                }
            }
            
            // SPECIAL CASE: Single character file
            if (distinct_chars == 1) {
                printf("\n⚠️  SPECIAL CASE: File contains only one unique character: ");
                
                // Afficher le caractère de manière lisible
                if (most_frequent_char == '\n') {
                    printf("NEWLINE (\\n)\n");
                } else if (most_frequent_char == '\r') {
                    printf("CARRIAGE RETURN (\\r)\n");
                } else if (most_frequent_char == '\t') {
                    printf("TAB (\\t)\n");
                } else if (most_frequent_char == ' ') {
                    printf("SPACE\n");
                } else if (most_frequent_char >= 32 && most_frequent_char <= 126) {
                    printf("'%c'\n", most_frequent_char);
                } else {
                    printf("ASCII code %d (non-printable)\n", most_frequent_char);
                }
                
                // Ask for confirmation
                printf("This file contains only this character repeated %d times.\n", size);
                printf("Trivial compression: All bits will be 0. Continue? (y/n): ");
                char confirm;
                scanf(" %c", &confirm);
                if (confirm != 'y' && confirm != 'Y') {
                    printf("Compression cancelled.\n");
                    free(data);
                    continue;
                }
                
                // Trivial compression for single character
                printf("[4-9] Performing trivial compression...\n");
                
                // Create minimal tree with single leaf
                bt *tree = createNode(most_frequent_char, size, NULL, NULL);
                if (!tree) {
                    printf("ERROR: Cannot create Huffman tree!\n");
                    free(data);
                    continue;
                }
                
                // Open output file
                FILE *out = fopen(output_path, "wb");
                if (!out) {
                    printf("ERROR: Cannot create output file '%s'!\n", output_path);
                    free(data);
                    freeTree(tree);
                    continue;
                }
                
                // Write original size
                fwrite(&size, sizeof(int), 1, out);
                
                // Special flag for "single character"
                unsigned char special_flag = 0xFF;
                fwrite(&special_flag, 1, 1, out);
                fwrite(&most_frequent_char, 1, 1, out);
                
                fclose(out);
                
                // Cleanup
                free(data);
                freeTree(tree);
                
                printf("\n=================================\n");
                printf("✅ SUCCESS: Trivial compression completed!\n");
                printf("Original:  %s (%d bytes)\n", input_path, size);
                printf("Compressed: %s (contains only metadata)\n", output_path);
                printf("=================================\n\n");
                
                continue; // Skip to next iteration
            }
            
            // NORMAL CASE: At least 2 different characters
            printf("Distinct characters found: %d\n", distinct_chars);
            
            // Create priority queue
            printf("[4] Creating priority queue...\n");
            queue *q = createQueue(freq);
            if (!q) {
                printf("ERROR: Failed to create priority queue!\n");
                free(data);
                continue;
            }
            
            // Create Huffman tree
            printf("[5] Building Huffman tree...\n");
            bt *tree = createHuffmanTree(q);
            if (!tree) {
                printf("ERROR: Failed to build Huffman tree!\n");
                free(data);
                freeQueue(q);
                continue;
            }
            
            // Generate Huffman codes
            printf("[6] Generating Huffman codes...\n");
            char *codes[256] = {NULL};
            char buffer[256];
            generateCodes(tree, codes, buffer, 0);
            
            // Compress data
            printf("[7] Compressing data...\n");
            char *compressed = compressData(data, size, codes);
            if (!compressed) {
                printf("ERROR: Compression failed!\n");
                free(data);
                freeQueue(q);
                freeTree(tree);
                for (int i = 0; i < 256; i++) if (codes[i]) free(codes[i]);
                continue;
            }
            
            // Calculate compression ratio
            int compressed_bits = strlen(compressed);
            int compressed_bytes = (compressed_bits + 7) / 8;
            float ratio = (1.0 - (float)compressed_bytes / size) * 100;
            
            // Write compressed file
            printf("[8] Writing compressed file '%s'...\n", output_path);
            FILE *out = fopen(output_path, "wb");
            if (!out) {
                printf("ERROR: Cannot create output file '%s'!\n", output_path);
                free(data);
                free(compressed);
                freeQueue(q);
                freeTree(tree);
                for (int i = 0; i < 256; i++) if (codes[i]) free(codes[i]);
                continue;
            }
            
            // Write original size
            fwrite(&size, sizeof(int), 1, out);
            
            // Write normal flag (not special)
            unsigned char normal_flag = 0x00;
            fwrite(&normal_flag, 1, 1, out);
            
            // Write Huffman tree
            saveTree(out, tree);
            
            // Write compressed data
            writeCompressed(out, compressed);
            
            fclose(out);
            
            // Cleanup
            printf("[9] Cleaning up memory...\n");
            for (int i = 0; i < 256; i++) {
                if (codes[i]) free(codes[i]);
            }
            free(data);
            free(compressed);
            freeTree(tree);
            
            printf("\n=================================\n");
            printf("✅ SUCCESS: Compression completed!\n");
            printf("Original:    %s (%d bytes)\n", input_path, size);
            printf("Compressed:  %s\n", output_path);
            printf("Compression: %d bits -> %d bytes\n", compressed_bits, compressed_bytes);
            printf("Ratio:       %.1f%% size reduction\n", ratio);
            printf("=================================\n\n");
            
        } else if (choice == 2) {
            // ========== DECOMPRESSION ==========
            printf("\n--- FILE DECOMPRESSION ---\n");
            printf("Enter the compressed file (e.g., compressed.huf): ");
            scanf("%255s", input_path);
            
            // CORRECTION : Vérifier les espaces
            if (strchr(input_path, ' ') != NULL) {
                printf("ERROR: File name contains spaces! Please use filename without spaces.\n");
                continue;
            }
            
            printf("Enter name for decompressed file (e.g., decompressed.txt): ");
            scanf("%255s", output_path);
            
            printf("\n[1] Opening compressed file '%s'...\n", input_path);
            FILE *in = fopen(input_path, "rb");
            if (!in) {
                printf("ERROR: Cannot open file '%s'!\n", input_path);
                printf("Make sure the file exists and name is correct.\n");
                continue;
            }
            
            // Read original size
            printf("[2] Reading original file size...\n");
            int original_size;
            if (fread(&original_size, sizeof(int), 1, in) != 1) {
                printf("ERROR: Invalid compressed file format!\n");
                printf("This might not be a valid Huffman compressed file.\n");
                fclose(in);
                continue;
            }
            
            // Check size - if 0, it's an empty file
            if (original_size == 0) {
                printf("INFO: Compressed file indicates empty original file (0 bytes).\n");
                printf("Creating empty output file '%s'...\n", output_path);
                fclose(in);
                
                // Create truly empty file
                FILE *out = fopen(output_path, "wb");
                if (out) {
                    fclose(out);
                    printf("✅ SUCCESS: Empty file created: %s (0 bytes)\n", output_path);
                } else {
                    printf("ERROR: Cannot create output file!\n");
                }
                continue;
            }
            
            printf("Original file size: %d bytes\n", original_size);
            
            // Read special/normal flag
            unsigned char flag;
            if (fread(&flag, 1, 1, in) != 1) {
                printf("ERROR: Cannot read compression flag!\n");
                fclose(in);
                continue;
            }
            
            // SPECIAL CASE: Single character
            if (flag == 0xFF) {
                printf("[3] Detected single-character compressed file...\n");
                
                // Read the single character
                unsigned char single_char;
                if (fread(&single_char, 1, 1, in) != 1) {
                    printf("ERROR: Cannot read single character!\n");
                    fclose(in);
                    continue;
                }
                
                // Display character info
                printf("Character: ");
                if (single_char == '\n') {
                    printf("newline (\\n)\n");
                } else if (single_char == '\r') {
                    printf("carriage return (\\r)\n");
                } else if (single_char == '\t') {
                    printf("tab (\\t)\n");
                } else if (single_char == ' ') {
                    printf("space\n");
                } else if (single_char >= 32 && single_char <= 126) {
                    printf("'%c'\n", single_char);
                } else {
                    printf("ASCII code %d\n", single_char);
                }
                
                // Reconstruct original file
                printf("[4] Reconstructing file with %d occurrences...\n", original_size);
                
                unsigned char *decompressed = (unsigned char *)malloc(original_size);
                if (!decompressed) {
                    printf("ERROR: Memory allocation failed!\n");
                    fclose(in);
                    continue;
                }
                
                // Fill with the single character
                for (int i = 0; i < original_size; i++) {
                    decompressed[i] = single_char;
                }
                
                // Write file
                printf("[5] Writing decompressed file '%s'...\n", output_path);
                FILE *out = fopen(output_path, "wb");
                if (!out) {
                    printf("ERROR: Cannot create output file!\n");
                    free(decompressed);
                    fclose(in);
                    continue;
                }
                
                writeFile(out, decompressed, original_size);
                fclose(out);
                
                // Cleanup
                free(decompressed);
                fclose(in);
                
                printf("\n=================================\n");
                printf("✅ SUCCESS: Decompression completed!\n");
                printf("Reconstructed: %s (%d bytes, all same character)\n", output_path, original_size);
                printf("=================================\n\n");
                
                continue;
            }
            
            // NORMAL CASE: Multiple characters
            if (flag != 0x00) {
                printf("WARNING: Unknown flag (0x%02X). Trying normal decompression...\n", flag);
            }
            
            printf("[3] Rebuilding Huffman tree...\n");
            bt *tree = loadTree(in);
            if (!tree) {
                printf("ERROR: Cannot rebuild Huffman tree!\n");
                fclose(in);
                continue;
            }
            
            // Decompress data
            printf("[4] Decompressing data...\n");
            unsigned char *decompressed = decompressData(in, tree, original_size);
            fclose(in);
            
            if (!decompressed) {
                printf("ERROR: Decompression failed!\n");
                freeTree(tree);
                continue;
            }
            
            // Write decompressed file
            printf("[5] Writing decompressed file '%s'...\n", output_path);
            FILE *out = fopen(output_path, "wb");
            if (!out) {
                printf("ERROR: Cannot create output file '%s'!\n", output_path);
                freeTree(tree);
                free(decompressed);
                continue;
            }
            
            writeFile(out, decompressed, original_size);
            fclose(out);
            
            // Cleanup
            printf("[6] Cleaning up memory...\n");
            freeTree(tree);
            free(decompressed);
            
            printf("\n=================================\n");
            printf("✅ SUCCESS: Decompression completed!\n");
            printf("Compressed:  %s\n", input_path);
            printf("Decompressed: %s (%d bytes)\n", output_path, original_size);
            printf("=================================\n\n");
            
        } else if (choice == 3) {
            printf("\nThank you for using Huffman Coding!\n");
            printf("Program exiting...\n");
            break;
            
        } else {
            printf("\nERROR: Invalid choice! Please enter 1, 2, or 3.\n");
        }
    }
    
    return 0;
}