import os
from gitpandas.repository import Repository
# get the path of this repo
path = os.path.abspath('/home/lukas/Desktop/zpath/git-pandas')
# build an example repository object and try some things out
ignore_dirs = ['tests']
r = Repository(path, verbose=True)
# get the hours estimate for this repository (using 30 mins per commit)
he = r.hours_estimate(
    branch='master',
    grouping_window=0.5,
    single_commit_hours=0.5,
    limit=None,
    extensions=['cpp', 'h'],
    ignore_dir=ignore_dirs
)
print(he)