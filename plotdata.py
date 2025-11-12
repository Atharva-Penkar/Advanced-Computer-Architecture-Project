import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

CSV_PATH = "./ACAprojectdata.csv"

def load_and_preprocess_data(csv_path):
    try:
        df = pd.read_csv(csv_path)
    except FileNotFoundError:
        print(f"Error: CSV file not found at {csv_path}")
        return None
    except Exception as e:
        print(f"Error loading CSV: {e}")
        return None

    dl1_cols = [col for col in df.columns if '_dl1_hit_rate' in col]
    id_vars = ['nsets', 'bsize', 'assoc', 'repl', 'prefetcher']

    df_long = df.melt(
        id_vars=id_vars,
        value_vars=dl1_cols,
        var_name='Benchmark_Type',
        value_name='DL1_Hit_Rate'
    )

    df_long['Benchmark'] = df_long['Benchmark_Type'].str.replace('_dl1_hit_rate', '', regex=False)
    df_long['DL1_Hit_Rate'] = pd.to_numeric(df_long['DL1_Hit_Rate'], errors='coerce')
    return df_long.dropna(subset=['DL1_Hit_Rate'])


def plot_grouped_comparison(df, title, x_col, hue_col, filename, y_label):
    plot_data = df.groupby([x_col, hue_col], observed=False)['DL1_Hit_Rate'].mean().reset_index()

    plt.figure(figsize=(12, 6))
    groups = plot_data[x_col].unique()
    hues = plot_data[hue_col].unique()
    n_groups = len(groups)
    n_hues = len(hues)
    bar_width = 0.8 / n_hues
    x_pos = np.arange(n_groups)
    colors = plt.colormaps['viridis'](np.linspace(0, 1, n_hues))

    for i, hue in enumerate(hues):
        subset = plot_data[plot_data[hue_col] == hue]
        subset_ordered = [subset[subset[x_col] == g]['DL1_Hit_Rate'].iloc[0] if g in subset[x_col].values else 0 for g in groups]
        bar_x = x_pos + i * bar_width - (n_hues - 1) * bar_width / 2
        plt.bar(bar_x, subset_ordered, bar_width, label=hue, color=colors[i], edgecolor='black')

    y_min = plot_data['DL1_Hit_Rate'].min()
    y_max = plot_data['DL1_Hit_Rate'].max()
    margin = (y_max - y_min) * 0.2 if y_max > y_min else 0.001
    plt.ylim(y_min - margin, y_max + margin)

    plt.title(title, fontsize=14)
    plt.xlabel(x_col, fontsize=12)
    plt.ylabel(y_label, fontsize=12)
    plt.xticks(x_pos, groups, rotation=15, ha='right', fontsize=10)
    plt.legend(title=hue_col.capitalize(), bbox_to_anchor=(1.05, 1), loc='upper left')
    plt.grid(axis='y', linestyle='--', alpha=0.7)
    plt.tight_layout()
    plt.savefig(filename)


def plot_simple_comparison(df, title, x_col, filename, y_label, normalize=False):
    plot_data = df.groupby(x_col, observed=False)['DL1_Hit_Rate'].mean().reset_index()

    plt.figure(figsize=(10, 6))
    x_pos = np.arange(len(plot_data))
    colors = plt.colormaps['viridis'](np.linspace(0, 1, len(plot_data)))

    plt.bar(x_pos, plot_data['DL1_Hit_Rate'], width=0.8, color=colors, edgecolor='black')
    plt.title(title, fontsize=14)
    plt.xlabel(x_col, fontsize=12)
    plt.ylabel(y_label, fontsize=12)

    if normalize:
        y_min = plot_data['DL1_Hit_Rate'].min()
        y_max = plot_data['DL1_Hit_Rate'].max()
        margin = (y_max - y_min) * 0.2 if y_max > y_min else 0.001
        plt.ylim(y_min - margin, y_max + margin)
    else:
        plt.ylim(0, 1.0)

    plt.xticks(x_pos, plot_data[x_col], rotation=15, ha='right')
    plt.grid(axis='y', linestyle='--', alpha=0.7)
    plt.tight_layout()
    plt.savefig(filename)


def plot_replacement_vs_prefetcher(df, filename="plot_replacement_vs_prefetcher.png"):
    """
    Creates grouped bars for each replacement policy, showing the DL1 hit rate
    for each prefetcher (none, next_line, stride).
    """
    plot_data = df.groupby(['repl', 'prefetcher'], observed=False)['DL1_Hit_Rate'].mean().reset_index()

    plt.figure(figsize=(12, 6))
    repls = plot_data['repl'].unique()
    prefetchers = plot_data['prefetcher'].unique()
    n_repls = len(repls)
    n_pref = len(prefetchers)
    bar_width = 0.8 / n_pref
    x_pos = np.arange(n_repls)
    colors = plt.colormaps['viridis'](np.linspace(0, 1, n_pref))

    for i, pref in enumerate(prefetchers):
        subset = plot_data[plot_data['prefetcher'] == pref]
        subset_ordered = [subset[subset['repl'] == r]['DL1_Hit_Rate'].iloc[0] if r in subset['repl'].values else 0 for r in repls]
        bar_x = x_pos + i * bar_width - (n_pref - 1) * bar_width / 2
        plt.bar(bar_x, subset_ordered, bar_width, label=pref, color=colors[i], edgecolor='black')

    y_min = plot_data['DL1_Hit_Rate'].min()
    y_max = plot_data['DL1_Hit_Rate'].max()
    margin = (y_max - y_min) * 0.2 if y_max > y_min else 0.001
    plt.ylim(y_min - margin, y_max + margin)

    plt.title("2. DL1 Hit Rate by Replacement Policy and Prefetcher (Across All Benchmarks)", fontsize=14)
    plt.xlabel("Replacement Policy", fontsize=12)
    plt.ylabel("Average DL1 Hit Rate", fontsize=12)
    plt.xticks(x_pos, repls, fontsize=10)
    plt.legend(title="Prefetcher", bbox_to_anchor=(1.05, 1), loc='upper left')
    plt.grid(axis='y', linestyle='--', alpha=0.7)
    plt.tight_layout()
    plt.savefig(filename)


def plot_benchmark_breakdown(df, filename="plot_benchmark_breakdown_fixed.png"):
    df['Policy_Pref'] = df['repl'].str.upper() + " / " + df['prefetcher']
    plot_grouped_comparison(
        df,
        'DL1 Hit Rate Breakdown by Benchmark and Configuration',
        x_col='Benchmark',
        hue_col='Policy_Pref',
        filename=filename,
        y_label='DL1 Hit Rate'
    )


def plot_assoc_replacement_benchmark(df, filename="plot_assoc_repl_benchmark_comparison.png"):
    df['Assoc_Repl'] = df['assoc'].astype(str) + "-way / " + df['repl'].str.upper()
    plot_grouped_comparison(
        df,
        "DL1 Hit Rate by Benchmark Across Associativities and Replacement Policies",
        x_col="Benchmark",
        hue_col="Assoc_Repl",
        filename=filename,
        y_label="DL1 Hit Rate"
    )


if __name__ == "__main__":
    df_long = load_and_preprocess_data(CSV_PATH)
    if df_long is not None:
        # 1. Prefetcher comparison (normalized)
        plot_simple_comparison(
            df_long,
            "1. Average DL1 Hit Rate by Prefetcher Policy (Across All Benchmarks)",
            'prefetcher',
            filename="plot_1_prefetcher_comparison_normalized.png",
            y_label="Average DL1 Hit Rate",
            normalize=True
        )

        # 2. Replacement policy grouped by prefetcher
        plot_replacement_vs_prefetcher(
            df_long,
            filename="plot_2_replacement_vs_prefetcher.png"
        )

        # 3. Benchmark breakdown
        plot_benchmark_breakdown(
            df_long,
            filename="plot_3_benchmark_breakdown_fixed.png"
        )

        # 4. Associativity + replacement comparison
        plot_assoc_replacement_benchmark(
            df_long,
            filename="plot_4_assoc_repl_benchmark_comparison.png"
        )

        print("\nAnalysis complete. 4 plots have been generated.")
