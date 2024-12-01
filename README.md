# Spell Checker Project

## **Overview**

This project is a fast and efficient spell checker that I developed. It includes both the main program logic and the dictionary management functions. The spell checker uses a **hash table** to store dictionary words for quick lookups and checks the spelling of words in a text file, reporting any misspellings. The program is optimized for real-world performance and ensures memory safety.

---

## **Features**
1. **Efficient Word Lookup**: Utilizes a custom hash table for rapid word checking.
2. **Case-Insensitive Matching**: Recognizes words regardless of capitalization (e.g., `Apple` and `apple` are treated equally).
3. **Memory Optimization**: Dynamically allocated memory is properly freed to prevent leaks.
4. **Customizable Hash Table**: The number of hash buckets can be adjusted for better performance depending on dictionary size.

---

## **Project Structure**

### **Files**
- **`speller.c`**: The main program that handles:
  - Loading the dictionary.
  - Reading and processing the text file.
  - Passing words to the dictionary logic for checking.
  - Reporting misspelled words and execution statistics.
- **`dictionary.c`**: Contains all the dictionary logic I wrote, including:
  - Loading the dictionary into memory.
  - Hashing words for the hash table.
  - Checking if a word exists.
  - Managing memory by unloading the hash table.
- **`dictionary.h`**: The header file with constants and function declarations shared between `speller.c` and `dictionary.c`.
- **`Makefile`**: Automates compilation of the project.
- **`dictionaries/`**:
  - **`large`**: A comprehensive dictionary with 143,091 words.
  - **`small`**: A smaller dictionary I used for testing and debugging.
- **`texts/`**: Contains various text files I tested the spell checker with.
- **`keys/`**: Contains answer keys to verify the spell checker's output.

---

## **Implementations**

### **1. Hash Table**
- Implemented the dictionary using a **hash table** to achieve fast word lookups.
- Each bucket in the hash table contains a **linked list** to handle collisions.

### **2. Custom Hash Function**
- Created a custom hash function based on the **djb2 algorithm**. It ensures an even distribution of words across the hash table, reducing collisions and improving lookup speed.

### **3. Memory Management**
- All memory allocated for the hash table and linked lists is freed when the program finishes using the `unload` function. I verified the program's memory safety using **Valgrind**.

### **4. Case-Insensitive Matching**
- The program handles words regardless of capitalization by converting them to lowercase before processing.

### **5. Main Program Logic**
- Designed the main program in `speller.c` to:
  - Handle command-line arguments for selecting the dictionary and text files.
  - Read words from the text file and pass them to the dictionary logic for checking.
  - Log misspelled words and output execution statistics (e.g., time taken for loading, checking, unloading).
- **Error Handling:**
  - Ensures the program gracefully handles missing files or invalid arguments.
  - Outputs clear error messages when files cannot be opened.

---

## **Usage Instructions**

### **1. Compiling the Program**
1. Open a terminal and navigate to the project directory.
2. Run the following command to compile the program:
   ```bash
   make
   ```
3. This will create an executable named `speller`.

### **2. Running the Spell Checker**
The program can be run with the following syntax:
```bash
./speller [dictionary] text
```

- **`dictionary`**: Path to the dictionary file (optional). If not provided, it defaults to `dictionaries/large`.
- **`text`**: Path to the text file to spell-check.

#### **Examples**
1. Spell check `texts/lalaland.txt` using the default dictionary:
   ```bash
   ./speller texts/lalaland.txt
   ```
2. Spell check `texts/cat.txt` using the small dictionary:
   ```bash
   ./speller dictionaries/small texts/cat.txt
   ```

### **3. Interpreting the Output**
The program displays:
- A list of misspelled words.
- Statistics about:
  - Total words misspelled.
  - Number of words in the dictionary.
  - Total words in the text.
  - Time taken for each operation (loading, checking, etc.).

---

## **Testing My Implementation**

### **1. Validating Output**
- To verify the correctness of the program, I compared its output to provided answer keys in the `keys/` directory.

#### **Steps:**
1. Run the program and redirect the output to a file:
   ```bash
   ./speller texts/lalaland.txt > student.txt
   ```
2. Compare the output to the corresponding answer key:
   ```bash
   diff -y student.txt keys/lalaland.txt
   ```
3. Differences (if any) are marked with symbols like `>` or `<`.

### **2. Checking Memory Safety**
I used **Valgrind** to ensure the program doesn't leak memory:
```bash
valgrind --leak-check=full ./speller dictionaries/small texts/cat.txt
```
- Valgrind reports showed no memory leaks, confirming the correctness of my implementation.

---

## **Challenges and Solutions**

### **1. Designing the Hash Function**
- **Challenge:** Ensuring words are evenly distributed across the hash table.
- **Solution:** I used the **djb2 algorithm**, which combines bit shifting and ASCII values to generate hash values. This minimized collisions and improved performance.

### **2. Managing Memory**
- **Challenge:** Preventing memory leaks during node creation and unloading.
- **Solution:** I carefully implemented the `unload` function, freeing every allocated node, and verified this using Valgrind.

### **3. Error Handling**
- **Challenge:** Ensuring the program handled missing or invalid files gracefully.
- **Solution:** I added checks for invalid command-line arguments and file I/O errors in `speller.c`, providing user-friendly error messages.

---

## **Optimization Choices**

### **1. Adjusting Hash Table Size**
- Initially, the table size (`N`) was set to 26, matching the number of letters in the alphabet. However, I increased it to 100,000 to reduce collisions and improve performance for larger dictionaries.

### **2. Precomputing Word Count**
- Instead of counting words dynamically, I maintained a counter while loading the dictionary, enabling the `size` function to run in constant time.

---

## **Example Commands**

### **Compile the Program**
```bash
make
```

### **Run the Spell Checker**
```bash
./speller texts/cat.txt
```

### **Compare Outputs**
```bash
diff -y student.txt keys/cat.txt
```

### **Check for Memory Leaks**
```bash
valgrind --leak-check=full ./speller dictionaries/small texts/cat.txt
```

---

## **Lessons Learned**

- **Efficiency Matters:** Designing the right hash function and managing collisions effectively are critical for performance.
- **Memory Management:** Proper allocation and deallocation are essential in C programming, especially in large-scale projects.
- **Breaking Down Problems:** Developing the project incrementally made debugging and testing much more manageable.

---

## **Conclusion**

This project was a great opportunity to learn about:
- File handling and dynamic memory management in C.
- Data structures like hash tables and linked lists.
- Designing efficient algorithms and optimizing real-world performance.
