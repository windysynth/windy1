
// some code snippets


//-----------------------
//  pass pointer version
//-----------------------

#include <iostream>
#include <cstdint> // Use cstdint for uint8_t

// Correct function pointer type: takes a pointer to uint8_t
typedef bool (*Item_Function)(uint8_t*);

// The struct holds a pointer to the variable
typedef struct MenuItem_t
{
  char text[16];
  Item_Function func;
  uint8_t* arg; // Now a pointer to the argument
} MenuItem;

// The functions must now accept a pointer
bool funcOneAndtwo(uint8_t* var_ptr) {
    if (var_ptr) {
        *var_ptr = 2 * (*var_ptr);
        return true;
    }
    return false;
}

bool funcThree(uint8_t* var_ptr) {
    if (var_ptr) {
        *var_ptr = 3 * (*var_ptr);
        return true;
    }
    return false;
}

uint8_t var1;
uint8_t var2;
uint8_t var3;

MenuItem testMenu[3] = {
    {"NameOne ", funcOneAndtwo, &var1}, // Store the address of var1
    {"NameTwo ", funcOneAndtwo, &var2}, // Store the address of var2
    {"NameThree ", funcThree, &var3}  // Store the address of var3
};

int main() {
    var1 = 41;
    var2 = 42;
    var3 = 43;
    
    // Call the function using the pointer stored in the struct
    bool test1 = testMenu[0].func(testMenu[0].arg); // Access with dot operator, pass pointer
    
    //static_cast<int>(...) is used to ensure std::cout prints the numerical value instead of a character. 
    std::cout << "var1 = " << static_cast<int>(var1) << std::endl;
    
    bool test2 = testMenu[1].func(testMenu[1].arg);
    std::cout << "var2 = " << static_cast<int>(var2) << std::endl;
    
    bool test3 = testMenu[2].func(testMenu[2].arg);
    std::cout << "var3 = " << static_cast<int>(var3) << std::endl;

    var1 = 31;
    var2 = 32;
    var3 = 33;
    
    test1 = testMenu[0].func(testMenu[0].arg);
    std::cout << "var1 = " << static_cast<int>(var1) << std::endl;
    
    test2 = testMenu[1].func(testMenu[1].arg);
    std::cout << "var2 = " << static_cast<int>(var2) << std::endl;
    
    test3 = testMenu[2].func(testMenu[2].arg);
    std::cout << "var3 = " << static_cast<int>(var3) << std::endl;
    
    return 0;
}

//-----------------------
//  by reference version
//-----------------------
#include <iostream>
#include <cstdint> // Use cstdint for uint8_t

// Correct function pointer type: takes a reference to uint8_t
typedef bool (*Item_Function)(uint8_t&);

// The struct holds a pointer to the variable, as references can't be reseated.
// A pointer is still necessary to store the "reference to the variable".
typedef struct MenuItem_t
{
  char text[16];
  Item_Function func;
  uint8_t* arg; // Still a pointer to the argument
} MenuItem;

// The functions now accept a reference
bool funcOneAndtwo(uint8_t& var_ref) {
    var_ref = 2 * var_ref; // Use directly without dereference
    return true;
}

bool funcThree(uint8_t& var_ref) {
    var_ref = 3 * var_ref; // Use directly without dereference
    return true;
}

uint8_t var1;
uint8_t var2;
uint8_t var3;

MenuItem testMenu[3] = {
    {"NameOne ", funcOneAndtwo, &var1}, // Store the address of var1
    {"NameTwo ", funcOneAndtwo, &var2}, // Store the address of var2
    {"NameThree ", funcThree, &var3}  // Store the address of var3
};

int main() {
    var1 = 41;
    var2 = 42;
    var3 = 43;

    // Call the function, dereferencing the stored pointer to get the reference
    bool test1 = testMenu[0].func(*testMenu[0].arg);
    
    std::cout << "var1 = " << static_cast<int>(var1) << std::endl;
    
    bool test2 = testMenu[1].func(*testMenu[1].arg);
    std::cout << "var2 = " << static_cast<int>(var2) << std::endl;
    
    bool test3 = testMenu[2].func(*testMenu[2].arg);
    std::cout << "var3 = " << static_cast<int>(var3) << std::endl;

    var1 = 31;
    var2 = 32;
    var3 = 33;
    
    test1 = testMenu[0].func(*testMenu[0].arg);
    std::cout << "var1 = " << static_cast<int>(var1) << std::endl;
    
    test2 = testMenu[1].func(*testMenu[1].arg);
    std::cout << "var2 = " << static_cast<int>(var2) << std::endl;
    
    test3 = testMenu[2].func(*testMenu[2].arg);
    std::cout << "var3 = " << static_cast<int>(var3) << std::endl;
    
    return 0;
}

