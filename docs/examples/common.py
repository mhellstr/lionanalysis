import seaborn as sns
import matplotlib.pyplot as plt
import numpy as np
import ase.io
import ase.visualize.plot
import os
import pandas as pd
from typing import Sequence, Union
from pathlib import Path
from scipy.optimize import curve_fit


def hist2df(filename: Union[os.PathLike, str]) -> pd.DataFrame:
    return pd.read_csv(filename, sep="\s+").iloc[:, :3]


def rdf2df(filename: Union[os.PathLike, str]) -> pd.DataFrame:
    return pd.read_csv(filename, sep="\s+").iloc[:, :6]


def averagesize2df(filename: os.PathLike, include_xxxx: bool = False) -> pd.DataFrame:
    """Converts averagesize.dat into a dataframe.

    Args:
        filename (os.PathLike): Path to averagesize.dat
        include_xxxx (bool, optional): If False, filter out all groups containing xxxx (groups created internally by lionanalysis). Defaults to False.

    Returns:
        pd.DataFrame: _description_
    """
    df = pd.read_csv(filename, sep="\s+", comment="#", names=("Group", "AverageSize"))
    if include_xxxx:
        return df

    return df.drop(df[df["Group"].str.contains("xxxx")].index)


def singlegroupproperty2df(filename: Union[os.PathLike, str], prefix: str = "col") -> pd.DataFrame:
    """Convert PrintProperties output for a single group with timestepiteration and per-atom properties to a pandas DataFrame.

    PrintProperties filename.txt GROUPS OneGroup PROPERTIES timestepiteration [id|coordinationnumber|...]

    :param filename: _description_
    :type filename: Union[os.PathLike, str]
    :param prefix: _description_, defaults to "col"
    :type prefix: str, optional
    """
    df = pd.read_csv(filename, sep="\s+", header=None, skiprows=1)
    df.columns = ["#iter"] + [f"{prefix}{i+1}" for i in range(df.shape[1] - 1)]
    return df


def msd2df(filename: Union[os.PathLike, str]) -> pd.DataFrame:
    df = pd.read_csv(filename, sep="\s+", skiprows=2)
    return df


def tcf2df(filename: Union[os.PathLike, str]) -> pd.DataFrame:
    df = pd.read_csv(filename, sep="\s+", skiprows=2)
    return df


def wip_seaborn_rdf_multiplot(rdf_files: Sequence[os.PathLike]):
    df = pd.concat(rdf2df(filename).assign(source=filename) for filename in rdf_files)
    sns.relplot(df, x="#r", y="RDF", hue="source", kind="line", height=3)


def plot_trajectory(
    filename: Union[os.PathLike[str], str],
    n_images: int = 3,
    plot_cell: bool = True,
    rotation: str = "-85x,-5y,0z",
    radii: float = 0.6,
) -> plt.Axes:
    """Plot a .xyz file.

    Args:
        filename (str): path to .xy file
        n_images (int, optional): number of images to plot (always includes first and last frame). Defaults to 3.

    Returns:
        _type_: plt.Axes
    """
    trajectory = ase.io.read(str(filename), ":")
    n_images = min(n_images, len(trajectory))
    # read in the lattice parameters from the mattixyz format
    lattice_line_counter = -1
    if plot_cell:
        with open(filename, "r") as f:
            for line in f:
                if line.startswith("XYZ "):
                    lattice_line_counter += 1
                    splitline = line.split()
                    trajectory[lattice_line_counter].cell = np.array(
                        [float(splitline[1]), float(splitline[2]), float(splitline[3])]
                    )
    else:
        for frame in trajectory:
            frame.cell = None
    frames = np.linspace(0, len(trajectory) - 1, num=n_images, endpoint=True, dtype=np.int64)
    fig, ax = plt.subplots(1, len(frames), figsize=(10, 3))
    basename = os.path.basename(filename)
    if n_images == 1:
        ax = [ax]
    for i, frame in enumerate(frames):
        ase.visualize.plot.plot_atoms(trajectory[frame], ax=ax[i], rotation=rotation, radii=radii)
        ax[i].set_title(f"{basename} #{frame+1}")
        ax[i].axis("off")
    return ax


def double_exp(x, A: float, tau1: float, tau2: float):
    return A * np.exp(-x / tau1) + (1 - A) * np.exp(-x / tau2)


def fit_and_plot_double_exp(
    df: pd.DataFrame, A: float = 0.1, tau1: float = 0.5, tau2: float = 2.5, title: str = ""
) -> plt.Axes:
    p0 = A, tau1, tau2
    x = df["#t(ps)"].to_numpy()
    y = df["value"].to_numpy()

    fig, ax = plt.subplots()
    try:
        popt, _ = curve_fit(double_exp, x, y, p0=p0)
        A, tau1, tau2 = popt
        prediction = double_exp(x, A, tau1, tau2)

        lifetime = A * tau1 + (1 - A) * tau2
        print(f"{A=:.2f} {tau1=:.2f} {tau2=:.2f} {lifetime=:.2f}")

        ax.plot(x, y, "-")
        ax.plot(x, prediction)
        ax.set_title(f"LIFETIME {title} = {lifetime:.2f} ps")
        ax.set_xlabel("Correlation time (ps)")
        ax.legend(["Correlation function", "Double-exponential fit"])
    except RuntimeError:
        ax.plot(x, y, "-")
        ax.set_title("Couldn't fit double exponential to this function.")

    return ax
