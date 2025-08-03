# -------------------------------
# Phase 1: Basic Python Programming
# -------------------------------

# 1. Variables and Data Types
name = "Alice"       # String (str)
age = 20             # Integer (int)
height = 1.65        # Floating-point number (float)
is_student = True    # Boolean (True/False)

print("Hello,", name)
print("Your age is:", age)
print("Height:", height, "m")
print("Are you a student?", is_student)

# 2. Conditional Statements (if-else)
if age >= 18:
    print("You are an adult.")
else:
    print("You are still young.")

# 3. Loops (for and while)
print("\nPrint numbers from 1 to 5 using for loop:")
for i in range(1, 6):
    print(i, end=" ")

print("\n\nPrint numbers from 5 down to 1 using while loop:")
count = 5
while count > 0:
    print(count, end=" ")
    count -= 1

# 4. Functions
def greet(user_name):
    """A simple function to return a greeting"""
    return f"Hello {user_name}!"

print("\n\n", greet(name))

# 5. Classes and Objects
class Student:
    def __init__(self, name, age):
        self.name = name
        self.age = age
    
    def introduce(self):
        print(f"My name is {self.name}, and I am {self.age} years old.")

# Create a student object
student1 = Student("Nguyễn Thái Thuận", 22)
student1.introduce()
