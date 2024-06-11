import subprocess
import os
from params import parameters

def run_program(executable, args):
    """
    运行指定的可执行文件并传递参数
    """
    command = [executable] + args
    subprocess.run(command)

def main():
    executable = "/app/proj/build/bin/generate_trace"  # 替换为你的可执行文件路径
    log_dir = "/app/proj/data_analysis"

    # p_good->bad, p_bad_to_good, loss_good, loss_bad  
    

    # 生成 6 个不同的 trace
    for params in parameters:
        # 日志文件前缀作为程序参数
        tmp = "_".join(params)
        log_file_prefix = f"trace_{tmp}"
        args = [log_file_prefix] + params
        print(log_file_prefix, args)
        # 运行程序
        run_program(executable, args)
        import time
        time.sleep(61)

if __name__ == "__main__":
    main()
