import pandas as pd
import matplotlib.pyplot as plt
from pathlib import Path

def get_normalized_data(filename='q5_normalized_perf_data.csv'):
    # Define the file path using pathlib
    file_path = Path(filename)

    # Check if the file exists
    if file_path.exists():
        # Read the normalized DataFrame from the file
        data = pd.read_csv(file_path)
        print("Normalized data loaded from file.")
    else:
        # Read the data from clipboard
        data = pd.read_clipboard()

        # Reset the index to turn the multi-index into columns
        data = data.reset_index()

        # Rename the columns
        data.columns = ['alpha', 'operation', 'Chained Hashing', 'Linear Probing', 'Robin Hood Hashing']

        # Fill NaN values in the 'alpha' column with the last non-NaN value
        data['alpha'] = data['alpha'].fillna(method='ffill')

        # Remove the 'α =' prefix from the 'alpha' column
        data['alpha'] = data['alpha'].str.replace('α = ', '')

        # Convert 'alpha' to float
        data['alpha'] = data['alpha'].astype(float)

        # Function to convert time string to float
        def convert_time(time_str):
            return float(time_str.replace('ns', ''))

        # Convert time strings to float for all hashing methods
        for column in ['Chained Hashing', 'Linear Probing', 'Robin Hood Hashing']:
            data[column] = data[column].apply(convert_time)

        # Reset the index
        data = data.reset_index(drop=True)

        # Save the normalized DataFrame to a CSV file
        data.to_csv(file_path, index=False)
        print("Normalized data saved to file.")

    return data

# Example usage:
# data = get_normalized_data()
# print(data.head())
def plot_hashing_performance(data, methods, operations):
    # Create a figure with subplots for each method
    fig, axes = plt.subplots(1, 3, figsize=(20, 6))
    fig.suptitle('Performance Comparison of Hashing Methods', fontsize=16)

    # Plot for each method
    for i, method in enumerate(methods):
        ax = axes[i]
        ax.set_title(method)
        ax.set_xlabel('α (Load Factor)')
        ax.set_ylabel('Time (ns)')
        
        # Plot for each operation
        for operation in operations:
            # Filter data for success and failure
            success_data = data[(data['operation'] == f"{operation} (success)")]
            failure_data = data[(data['operation'] == f"{operation} (failure)")]
            
            # Plot success and failure lines
            ax.plot(success_data['alpha'], success_data[method], marker='o', label=f"{operation} (success)")
            ax.plot(failure_data['alpha'], failure_data[method], marker='x', linestyle='--', label=f"{operation} (failure)")
        
        ax.legend()
        ax.grid(True)
    plt.tight_layout()
    plt.show()


def plot_hashing_performance2(data, methods, operations):
    """
    Plot hashing performance for given methods and operations.
    
    Parameters:
    data (pd.DataFrame): Normalized dataframe containing performance data
    methods (list): List of hashing methods to plot
    operations (list): List of operations to plot
    """
    # Set up the plot style
    # plt.style.use('seaborn')

    # Create a figure with subplots
    fig, axes = plt.subplots(len(methods), len(operations), figsize=(5*len(operations), 5*len(methods)))
    fig.suptitle('Performance Comparison of Hashing Methods and Operations', fontsize=16)

    # Flatten the axes array for easier iteration
    axes = axes.flatten()

    # Plot for each method and operation combination
    for i, (method, operation) in enumerate([(m, o) for m in methods for o in operations]):
        ax = axes[i]
        ax.set_title(f'{method} - {operation}')
        ax.set_xlabel('α (Load Factor)')
        ax.set_ylabel('Time (ns)')
        
        # Filter data for success and failure
        success_data = data[data['operation'] == f"{operation} (success)"]
        failure_data = data[data['operation'] == f"{operation} (failure)"]
        
        # Plot success and failure lines
        ax.plot(success_data['alpha'], success_data[method], color='green', marker='o', label='Success')
        ax.plot(failure_data['alpha'], failure_data[method], color='red', marker='x', linestyle='--', label='Failure')
        
        ax.legend()
        # use all appearing alpha values as xticks
        ax.set_xticks(data['alpha'].unique())
        ax.grid(True)

    plt.tight_layout()
    plt.show()

def plot_hashing_performance3(data, methods, operations):
    """
    Plot hashing performance for given methods and operations.
    
    Parameters:
    data (pd.DataFrame): Normalized dataframe containing performance data
    methods (list): List of hashing methods to plot
    operations (list): List of operations to plot
    """
    # Set up the plot style
    #plt.style.use('seaborn')

    # Create a figure with subplots
    fig, axes = plt.subplots(len(methods), len(operations), figsize=(5*len(operations), 5*len(methods)))
    fig.suptitle('Performance Comparison of Hashing Methods and Operations', fontsize=16)

    # Plot for each method and operation combination
    for i, method in enumerate(methods):
        for j, operation in enumerate(operations):
            ax = axes[i, j]
            ax.set_title(f'{method} - {operation}')
            ax.set_xlabel('α (Load Factor)')
            ax.set_ylabel('Time (ns)')
            
            # Filter data for success and failure
            success_data = data[(data['operation'] == f"{operation} (success)") & (data['alpha'].isin([0.5, 0.6, 0.7]))]
            failure_data = data[(data['operation'] == f"{operation} (failure)") & (data['alpha'].isin([0.5, 0.6, 0.7]))]
            
            # Plot success and failure lines
            ax.plot(success_data['alpha'], success_data[method], color='green', marker='o', label='Success')
            ax.plot(failure_data['alpha'], failure_data[method], color='red', marker='x', linestyle='--', label='Failure')
            
            ax.legend()
            ax.set_xticks([0.5, 0.6, 0.7])
            ax.set_xlim(0.45, 0.75)
            
            # Set y-axis to start from 0 and end at the next round 100
            y_max = max(success_data[method].max(), failure_data[method].max())
            ax.set_ylim(0, ((int(y_max) // 100) + 1) * 100)
            
            ax.grid(True)

    plt.tight_layout()
    plt.show()

# Example usage:
# Assuming 'data' is your normalized dataframe

# methods = ['Chained Hashing', 'Linear Probing', 'Robin Hood Hashing']
# operations = ['Insert', 'Lookup', 'Remove']
# plot_hashing_performance(data, methods, operations)

if __name__ == "__main__":
    data = get_normalized_data()
    # Assuming we've already normalized the dataframe as in the previous step
    # If not, run the normalization code first

    # List of methods and operations
    methods = ['Chained Hashing', 'Linear Probing', 'Robin Hood Hashing']
    operations = ['Insert', 'Remove', 'Lookup']

    # Set up the plot style
    # plt.style.use('seaborn')

    # Call the plotting function
    plot_hashing_performance2(data, methods, operations)