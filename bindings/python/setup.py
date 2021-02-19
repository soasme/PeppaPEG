import os.path
from setuptools import setup

dirname = os.path.dirname(os.path.abspath(__file__))

install_requires=['cffi>=1.12.0']

setup(
    name='PeppaPEG',
    version='0.1.0',
    description='A CFFI binding for Peppa PEG, an untra lightweight PEG parsing library.',
    author='Ju Lin',
    author_email='soasme@gmail.com',
    url='https://github.com/soasme/PeppaPEG',
    license='MIT',
    long_description=open(os.path.join(dirname, 'README.md')).read(),
    packages=['misaka'],
    classifiers = [
        'Development Status :: 1 - Planning',
        'Intended Audience :: Developers',
        'License :: OSI Approved :: MIT License',
        'Programming Language :: C',
        'Programming Language :: Python :: 3.5',
        'Programming Language :: Python :: 3.6',
        'Programming Language :: Python :: 3.7',
        'Programming Language :: Python :: 3.8',
        'Programming Language :: Python :: Implementation :: CPython',
        'Programming Language :: Python :: Implementation :: PyPy',
        'Topic :: Text Processing',
        'Topic :: Utilities'
    ],
    setup_requires=['cffi>=1.12.0'],
    install_requires=install_requires,
    cffi_modules=['build_ffi.py:ffi'],
)
