import pandas as pd
import matplotlib.pyplot as plt
from pathlib import Path

def get_normalized_data(filename='q5_normalized_perf_data.csv'):
    file_path = Path(filename)
    if file_path.exists():
        data = pd.read_csv(file_path)
        print("Normalized data loaded from file.")
    else:
        data = pd.read_clipboard()
        data = data.reset_index()
        data.columns = ['alpha', 'operation', 'Chained Hashing', 'Linear Probing', 'Robin Hood Hashing']
        data['alpha'] = data['alpha'].fillna(method='ffill')
        data['alpha'] = data['alpha'].str.replace('α = ', '')
        data['alpha'] = data['alpha'].astype(float)

        def convert_time(time_str):
            return float(time_str.replace('ns', ''))

        for column in ['Chained Hashing', 'Linear Probing', 'Robin Hood Hashing']:
            data[column] = data[column].apply(convert_time)
        data = data.reset_index(drop=True)
        data.to_csv(file_path, index=False)
        print("Normalized data saved to file.")
    return data


def plot_hashing_performance_per_method(data, methods, operations):
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


def plot_hashing_performance(data, methods, operations):
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
    fig, axes = plt.subplots(len(methods), len(operations), figsize=(4*len(operations), 4*len(methods)))
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
        ax.grid(True)

    plt.tight_layout()
    plt.savefig('hashing_performance.png')

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
    plot_hashing_performance(data, methods, operations)