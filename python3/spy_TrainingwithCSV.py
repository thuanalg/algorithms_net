import pandas as pd
import numpy as np
import random

def spy_GenerateCSVData(num_customers, filename):
    # Number of customers
    #num_customers = 50
    
    # 1. Customer names
    names = [f"Customer_{i:03d}" for i in range(1, num_customers+1)]
    
    # 2. Fake bank cards (masking the first 12 digits)
    bank_cards = [f"4111-xxxx-xxxx-{random.randint(1000,9999)}" for _ in range(num_customers)]
    
    # 3. Frequent items customers usually buy
    items_list = ["Milk", "Bread", "Rice", "Eggs", "Coffee", "Tea", "Meat", "Fish", "Snacks", "Fruits", "Vegetables"]
    purchases = [", ".join(random.sample(items_list, random.randint(2,5))) for _ in range(num_customers)]
    
    # 4. Average spending per purchase (VND)
    avg_spend = np.random.randint(100_000, 5_000_000, size=num_customers)
    
    # Create DataFrame
    df = pd.DataFrame({
        "Name": names,
        "BankCard": bank_cards,
        "FrequentItems": purchases,
        "AvgSpend(VND)": avg_spend
    })
    
    # Save to CSV
    df.to_csv("D:/x/customer_data.csv", index=False)
    
    print("Customer CSV file created: customer_data.csv")
    print(df.head())

spy_GenerateCSVData(10, "D:/x/customer_data.csv")