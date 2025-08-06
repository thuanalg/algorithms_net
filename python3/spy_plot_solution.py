import numpy as np
import matplotlib.pyplot as plt

# Parameters
T = 5                 # Time constant
a = 1 / T             # a = 1/T
x0 = 1                # Initial condition
t = np.linspace(0, 50, 1000)  # Time array
print(t)
# Solution x(t) = x0 * e^(a*t)
x = x0 * np.exp(a * t)
print(x)

# Plot
plt.figure(figsize=(16, 10))
plt.plot(t, x, label=r'$x(t) = e^{t/5}$')
plt.title('Solution of x\' = (1/T) x with T=5, x0=1')
plt.xlabel('Time t (seconds)')
plt.ylabel('x(t)')
plt.grid(True)
plt.legend()
#plt.tight_layout()
plt.show()
print("end")