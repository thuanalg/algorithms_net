import numpy as np
import matplotlib.pyplot as plt
from sklearn.linear_model import LinearRegression, LogisticRegression

# ---------------------
# 1️⃣ Linear Regression Demo
# ---------------------

# Sample linear data
X_linear = np.array([[1], [2], [3], [4], [5]])
y_linear = np.array([1.2, 2.0, 2.8, 4.1, 5.1])

# Train the model
linear_model = LinearRegression()
linear_model.fit(X_linear, y_linear)

# Prediction for smooth curve
X_test = np.linspace(0, 6, 100).reshape(-1, 1)
y_pred_linear = linear_model.predict(X_test)

# Plot Linear Regression
plt.figure(figsize=(12, 5))
plt.subplot(1, 2, 1)
plt.scatter(X_linear, y_linear, color='blue', label='Data')
plt.plot(X_test, y_pred_linear, color='red', label='Linear Regression')
plt.title('Linear Regression (Predicting Continuous Values)')
plt.xlabel('X')
plt.ylabel('Y')
plt.legend()
plt.grid(True)

# ---------------------
# 2️⃣ Logistic Regression Demo
# ---------------------

# Sample binary classification data
X_log = np.array([[1], [2], [3], [4], [5]])
y_log = np.array([0, 0, 0, 1, 1])

# Train the Logistic Regression model
log_model = LogisticRegression()
log_model.fit(X_log, y_log)

# Compute sigmoid probabilities
y_prob = log_model.predict_proba(X_test)[:, 1]

# Plot Logistic Regression
plt.subplot(1, 2, 2)
plt.scatter(X_log, y_log, color='blue', label='Data')
plt.plot(X_test, y_prob, color='green', label='Logistic Regression (Sigmoid)')
plt.title('Logistic Regression (Binary Classification)')
plt.xlabel('X')
plt.ylabel('Probability of y=1')
plt.ylim(-0.1, 1.1)
plt.legend()
plt.grid(True)

plt.show()
