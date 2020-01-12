import setuptools
with open("README.md", "r") as fh:
    long_description = fh.read()

setuptools.setup(
     name='Remote Bridge Crusher Control',  
     version='0.1',
     author="Joel Newman",
     author_email="JoellNewman@hotmail.com",
     description="A web interface for EGBC Victoria Bridge Crusher",
     long_description=long_description,
     long_description_content_type="text/markdown",
     url="https://github.com/newmanjoel/BridgeCrusher/tree/pip-testing/remote_bridge_control_pkg",
     packages=setuptools.find_packages(),
     install_requires=[
         'dash==1.7.0',
         'pyserial',
         'dash-daq'
     ],
     classifiers=[
         "Programming Language :: Python :: 3",
         "License :: OSI Approved :: MIT License",
         "Operating System :: Windows",
     ],
 )