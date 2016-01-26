# -*- coding: utf-8 -*-

try:
    from setuptools import setup
except ImportError:
    from distutils.core import setup


def readme():
    with open('README.rst') as f:
        return f.read()


config = {'name': 'antex_view',
        'version': '0.1',
        'description': 'Python module to plot antex files.',
        'long_description': readme(),
        'url': 'https://github.com/xanthospap/ngpt',
        'download_url': '',
        'author': 'xp, da, vz',
        'author_email': '''xanthos@mail.ntua.gr,
danast@mail.ntua.gr,
vanzach@survey.ntua.gr''',
        'license': '',
        'packages': ['antex_view'],
        'scripts': ['bin/antex_view.py'],
        'install_requires': ['numpy', 'matplotlib'],
        'include_package_data': True}

setup(**config)
