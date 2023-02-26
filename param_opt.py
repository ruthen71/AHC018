import statistics
import os
import joblib
import time
import optuna

import subprocess

import plotly.graph_objects as go


n_parallel = 48
n_files = 500


def calc_score_each(seed: int, s_pred: int, s_once: int, l_ratio: float, r_ratio: float, dir_param_c: float, dir_param_mag: float, dir_param_const: int):
    in_file = f"in/{seed:04}.txt"
    os.makedirs("tools/out", exist_ok=True)
    out_file = f"out/{seed:04}.txt"
    # cmd = f"cd tools && cargo run --release --bin vis {in_file} {out_file} 2> /dev/null"
    cmd = f"cd tools && cargo run --release --bin tester ../main {s_pred} {s_once} {l_ratio} {r_ratio} {dir_param_c} {dir_param_mag} {dir_param_const} < ./{in_file} > ./{out_file}"
    result = subprocess.run(cmd, shell=True, capture_output=True, text=True)
    return int(result.stderr.split()[-1])


def calc_scores(s_pred: int, s_once: int, l_ratio: float, r_ratio: float, dir_param_c: float, dir_param_mag: float, dir_param_const: int):
    scores = joblib.Parallel(n_jobs=n_parallel)(
        joblib.delayed(calc_score_each)(i, s_pred, s_once, l_ratio, r_ratio, dir_param_c, dir_param_mag, dir_param_const) for i in range(n_files)
    )
    return scores


def objective(trial: optuna.trial.Trial):
    start = time.time()
    s_pred = trial.suggest_int("s_pred", 80, 120)
    s_once = trial.suggest_int("s_once", 25, 40)
    l_ratio = trial.suggest_float("l_ratio", 0.75, 0.85)
    r_ratio = trial.suggest_float("r_ratio", 0, 0.3)
    dir_param_c = trial.suggest_float("dir_param_c", 0, 0.5)
    dir_param_mag = trial.suggest_float("dir_param_mag", 0, 0.1)
    dir_param_const = trial.suggest_int("dir_param_const", 0, 15)
    scores = calc_scores(s_pred, s_once, l_ratio, r_ratio, dir_param_c, dir_param_mag, dir_param_const)
    print(f"elapsed: {time.time() - start}")
    return statistics.mean(scores)


if __name__ == "__main__":
    os.system("g++ -O2 -std=c++17 main.cpp -o main")
    study = optuna.create_study(
        direction="minimize",
        storage="sqlite:///ahc018.db",
        study_name="tune_range_evening17_2",
        load_if_exists=True,
    )
    study.optimize(objective, n_trials=200)
    # print result
    print(study.best_params)
    print(study.best_value)
    print(study.best_trial)

    # visualize
    # contour plot
    fig = optuna.visualization.plot_contour(study)
    fig.write_html('image/image_evening/contour.html')  # save as html file
    fig.write_image('image/image_evening/contour.png')  # save as png file

    # Empirical Distribution Function Plot
    fig = optuna.visualization.plot_edf(study)
    fig.write_html('image/image_evening/edf.html')  # save as html file
    fig.write_image('image/image_evening/edf.png')  # save as png file

    # Optimization History Plot
    fig = optuna.visualization.plot_optimization_history(study)
    fig.write_html('image/image_evening/optimization_history.html')  # save as html file
    fig.write_image('image/image_evening/optimization_history.png')  # save as png file

    # parallel coordinate plot
    fig = optuna.visualization.plot_parallel_coordinate(study)
    fig.write_html('image/image_evening/parallel_coordinate.html')  # save as html file
    fig.write_image('image/image_evening/parallel_coordinate.png')  # save as png file

    # Hyperparameter importances
    fig = optuna.visualization.plot_param_importances(study)
    fig.write_html('image/image_evening/param_importance.html')  # save as html file
    fig.write_image('image/image_evening/param_importance.png')  # save as png file

    # slice
    fig = optuna.visualization.plot_slice(study)
    fig.write_html('image/image_evening/slice.html')  # save as html file
    fig.write_image('image/image_evening/slice.png')  # save as png file

# 376 442