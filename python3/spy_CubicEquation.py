import numpy as np
import matplotlib.pyplot as plt

# Cubic function: x^3 + 11x - 1
def cubic(x, c):
    return x**3  + c*x 

# Generate x values from -2 to 5
c = -100
x = np.linspace(-10, 10, 5000)
y = cubic(x, c)

# Compute real roots of the cubic equation
roots = np.roots([1, 0, c, 0])  # coefficients [a,b,c,d]
real_roots = roots[np.isreal(roots)].real  # keep only real roots

# Plot the cubic function
plt.figure(figsize=(8, 5))
plt.plot(x, y, label=r'$y = x^3 + cx $')

# Highlight the roots
plt.scatter(real_roots, cubic(real_roots, c), color='red', zorder=5, label='Roots')

# Draw X and Y axes
plt.axhline(0, color='black', linewidth=1)
plt.axvline(0, color='black', linewidth=1)

# Add labels and grid
plt.title("Cubic Equation and Its Roots")
plt.xlabel("x")
plt.ylabel("y")
plt.grid(True)
plt.legend()
plt.show()
print("End")