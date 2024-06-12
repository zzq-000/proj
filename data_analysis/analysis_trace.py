import sys
from collections import defaultdict
import seaborn as sns
import matplotlib.pyplot as plt
import pandas as pd
from params import parameters
from scipy.interpolate import PchipInterpolator
import numpy as np
import mplcursors
trace_name = None
def GetSeqence(filename:str) -> list[int]:
    sequences = []
    cnt = 0
    with open(filename, 'r') as f:
        for line in f:
            if cnt < 4:
                cnt += 1
                continue
            # print(line)
            v = int(line.split(' ')[-1])
            sequences.append(v)
    return sequences

def Analysis(sequences: list[int]) -> tuple[dict[int:int], dict[int:int]]:
    burst_dict = defaultdict(int)
    guard_dict = defaultdict(int)
    loss_happened_before = False
    burst_length = 0
    guard_space = 0
    first_guard_space = True
    first_burst_lost = True
    for val in sequences:
        if val == 0:
            if not loss_happened_before:
                if guard_space:
                    if first_guard_space:
                        first_guard_space = False
                    else:
                        guard_dict[guard_space] += 1     
                    guard_space = 0           
                loss_happened_before = True
                burst_length += 1
            else:
                burst_length += 1
        else:
            if loss_happened_before:
                if burst_length:
                    if first_burst_lost:
                        first_burst_lost = False
                    else:
                        burst_dict[burst_length] += 1
                    burst_length = 0
                loss_happened_before = False
                guard_space += 1
            else:
                guard_space += 1
    # print(dict(burst_dict), dict(guard_dict))
    draw_burst_frequency_hist(burst_dict)
    draw_burst_frequency_cdf(burst_dict)
    draw_guard_frequency_hist(guard_dict)
    draw_guard_frequency_cdf(guard_dict)
    # return dict(burst_dict), dict(guard_dict)

def draw_burst_frequency_cdf(data: dict[int, int]):
    column = 'Burst Length'  
    title = "Burst Length CDF"
    df = pd.DataFrame(list(data.items()), columns=[column, 'Frequency'])
    # 按 Burst Length 排序
    df = df.sort_values(by=column)

    # 计算累计频率
    df['Cumulative Frequency'] = df['Frequency'].cumsum()

    # 计算 CDF
    df['CDF'] = df['Cumulative Frequency'] / df['Frequency'].sum()

    # 生成平滑的曲线
    x = df[column]
    y = df['CDF']
    f = PchipInterpolator(x, y)
    x_smooth = np.linspace(x.min(), x.max(), 500)
    y_smooth = f(x_smooth)

    # 绘制平滑的 CDF 图
    plt.figure(figsize=(12, 6))
    ax = sns.lineplot(x=x_smooth, y=y_smooth, marker='o')
    plt.xlabel(column)
    plt.ylabel('CDF')
    plt.title('Burst Length CDF')
    plt.grid(True)
    # 标注 1%, 5%, 10%, 95% 分位点
    percentiles = [0.05, 0.10, 0.95]
    for p in percentiles:
        x_perc = np.interp(p, y_smooth, x_smooth)
        y_perc = p
        plt.axvline(x=x_perc, linestyle='--', color='r')
        plt.axhline(y=y_perc, linestyle='--', color='r')
        plt.text(x_perc, y_perc, f'({x_perc:.2f}, {int(p*100)}%)', color='r', 
                 verticalalignment='bottom', horizontalalignment='right')

    # 添加交互功能
    cursor = mplcursors.cursor(ax, hover=True)

    @cursor.connect("add")
    def on_add(sel):
        sel.annotation.set(text=f"({sel.target[0]:.2f}, {sel.target[1]:.2f})",
                           position=(0, 20),  # offset the text from the point
                           textcoords="offset points",
                           ha="center")

    plt.savefig(f"{trace_name}_burst_frequency_cdf.svg")
    plt.close()
    
def draw_burst_frequency_hist(data: dict[int, int]): 
    column = 'Burst Length'  
    title = "Burst Length Distribution"
    df = pd.DataFrame(list(data.items()), columns=[column, 'Frequency'])
    
    
    # 排序并提取前6项
    df = df.sort_values(by='Frequency', ascending=False).head(5)


    # 计算第6项及其后的合并频率
    other_frequency = sum(data.values()) - df['Frequency'].sum()

    # 添加合并后的第6项
    other_df = pd.DataFrame({column: ['6+'], 'Frequency': [other_frequency]})
    df = pd.concat([df, other_df], ignore_index=True)
    # 重新计算百分比
    total_frequency = df['Frequency'].sum()
    df['Percentage'] = (df['Frequency'] / total_frequency) * 100
    # 绘制频率直方图
    plt.figure(figsize=(12, 6))
    ax = sns.barplot(x=column, y='Percentage', hue=column, data=df, palette='viridis', dodge=False, legend=False)
    
    for p in ax.patches:
        height = p.get_height()
        ax.annotate(f'{height:.2f}%', 
                    xy=(p.get_x() + p.get_width() / 2, height), 
                    xytext=(0, 3),  # 3 points vertical offset
                    textcoords='offset points', 
                    ha='center', va='bottom')
        
    plt.xlabel(column)
    plt.ylabel('Percentage (%)')
    plt.title(title)
    plt.savefig(f"{trace_name}_burst_distribution.svg")
    plt.close()

def draw_guard_frequency_cdf(data: dict[int, int]):
    column = 'Guard Space'  
    title = "Guard Space CDF"
    df = pd.DataFrame(list(data.items()), columns=[column, 'Frequency'])
    
    df = df.sort_values(by=column)

    # 计算累计频率
    df['Cumulative Frequency'] = df['Frequency'].cumsum()

    # 计算 CDF
    df['CDF'] = df['Cumulative Frequency'] / df['Frequency'].sum()

    x = df[column]
    y = df['CDF']
    f = PchipInterpolator(x, y)
    x_smooth = np.linspace(x.min(), x.max(), 500)
    y_smooth = f(x_smooth)

    # 绘制平滑的 CDF 图
    plt.figure(figsize=(12, 6))
    ax = sns.lineplot(x=x_smooth, y=y_smooth, marker='o')
    plt.xlabel(column)
    plt.ylabel('CDF')
    plt.title('Burst Length CDF')
    plt.grid(True)
    # 标注 1%, 5%, 10%, 95% 分位点
    percentiles = [0.05, 0.10, 0.95]
    for p in percentiles:
        x_perc = np.interp(p, y_smooth, x_smooth)
        y_perc = p
        plt.axvline(x=x_perc, linestyle='--', color='r')
        plt.axhline(y=y_perc, linestyle='--', color='r')
        plt.text(x_perc, y_perc, f'{int(p*100)}% ({x_perc:.2f}, {y_perc:.2f})', color='r', 
                 verticalalignment='bottom', horizontalalignment='right')

    # 添加交互功能
    cursor = mplcursors.cursor(ax, hover=True)

    @cursor.connect("add")
    def on_add(sel):
        sel.annotation.set(text=f"({sel.target[0]:.2f}, {sel.target[1]:.2f})",
                           position=(0, 20),  # offset the text from the point
                           textcoords="offset points",
                           ha="center")

    plt.savefig(f"{trace_name}_guard_frequency_cdf.svg")
    plt.close()

def draw_guard_frequency_hist(data: dict[int, int]): 
    column = 'Guard Space'  
    title = "Guard Space Distribution"
    df = pd.DataFrame(list(data.items()), columns=[column, 'Frequency'])
    
    
    # 排序并提取前6项
    df = df.sort_values(by='Frequency', ascending=False).head(5)


    # 计算第6项及其后的合并频率
    other_frequency = sum(data.values()) - df['Frequency'].sum()

    # 添加合并后的第6项
    other_df = pd.DataFrame({column: ['6+'], 'Frequency': [other_frequency]})
    df = pd.concat([df, other_df], ignore_index=True)
    # 重新计算百分比
    total_frequency = df['Frequency'].sum()
    df['Percentage'] = (df['Frequency'] / total_frequency) * 100
    # 绘制频率直方图
    plt.figure(figsize=(12, 6))
    ax = sns.barplot(x=column, y='Percentage', hue=column, data=df, palette='viridis', dodge=False, legend=False)
    
    for p in ax.patches:
        height = p.get_height()
        ax.annotate(f'{height:.2f}%', 
                    xy=(p.get_x() + p.get_width() / 2, height), 
                    xytext=(0, 3),  # 3 points vertical offset
                    textcoords='offset points', 
                    ha='center', va='bottom')
        
    plt.xlabel(column)
    plt.ylabel('Percentage (%)')
    plt.title(title)
    plt.savefig(f"{trace_name}_guard_space.svg")
    plt.close()

if __name__ == "__main__":
    # trace_name = sys.argv[1]
    # Analysis(GetSeqence(trace_name))
    
    for params in parameters:
        # 日志文件前缀作为程序参数
        tmp = "_".join(params)
        trace_file_name = f"trace_{tmp}.INFO"
        
        trace_name = trace_file_name
        Analysis(GetSeqence(trace_file_name))
        
        