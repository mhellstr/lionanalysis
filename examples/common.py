import seaborn as sns
import matplotlib.pyplot as plt
import numpy as np
import ase.io
import ase.visualize.plot
import os
import pandas as pd
from typing import Sequence, Union
from pathlib import Path


def hist2df(filename: str) -> pd.DataFrame:
    return pd.read_csv(filename, sep='\s+').iloc[:, :4]


def rdf2df(filename: str) -> pd.DataFrame:
    return pd.read_csv(filename, sep='\s+').iloc[:, :6]


def averagesize2df(
        filename: os.PathLike,
        include_xxxx: bool = False
) -> pd.DataFrame:
    """Converts averagesize.dat into a dataframe.

    Args:
        filename (os.PathLike): Path to averagesize.dat
        include_xxxx (bool, optional): If False, filter out all groups containing xxxx (groups created internally by lionanalysis). Defaults to False.

    Returns:
        pd.DataFrame: _description_
    """
    df = pd.read_csv(
        filename,
        sep="\s+",
        comment="#",
        names=("Group", "AverageSize")
    )
    if include_xxxx:
        return df

    return df.drop(df[df["Group"].str.contains("xxxx")].index)


def wip_seaborn_rdf_multiplot(rdf_files: Sequence[Union[Path, str]]):
    df = pd.concat(rdf2df(filename).assign(source=filename) for filename in rdf_files)
    sns.relplot(df, x="#r", y="RDF", hue="source", kind="line", height=3)


def plot_trajectory(filename: str, n_images: int = 3) -> plt.Axes:
    """Plot a .xyz file.

    Args:
        filename (str): path to .xy file
        n_images (int, optional): number of images to plot (always includes first and last frame). Defaults to 3.

    Returns:
        _type_: plt.Axes
    """
    trajectory = ase.io.read(filename, ":")
    n_images = min(n_images, len(trajectory))
    # read in the lattice parameters from the mattixyz format
    lattice_line_counter = -1
    with open(filename, "r") as f:
        for line in f:
            if line.startswith("XYZ "):
                lattice_line_counter += 1
                splitline = line.split()
                trajectory[lattice_line_counter].cell = np.array([
                    float(splitline[1]),
                    float(splitline[2]),
                    float(splitline[3])
                ])
    frames = np.linspace(0, len(trajectory)-1, num=n_images, endpoint=True, dtype=np.int64)
    fig, ax = plt.subplots(1, len(frames), figsize=(10, 3))
    basename = os.path.basename(filename)
    if n_images == 1:
        ax = [ax]
    for i, frame in enumerate(frames):
        ase.visualize.plot.plot_atoms(trajectory[frame], ax=ax[i], rotation="-85x,-5y,0z", radii=0.6)
        ax[i].set_title(f"{basename} #{frame+1}")
        ax[i].axis("off")
    return ax
