import gvt
import numpy as np

# A python implementation of the GraviT volume renderer

gvt.gvtInit()

# Read some data

dims = (256,256,256)
Volume = np.reshape(np.fromfile("../data/vol/1000.256",dtype=np.int32),dims)

# Some volume metadata

# Deltas - cell spacing

deltas = np.array([1.0,1.0,1.0])

# Counts - number of points in each coordinate direction

counts = dims

# Origin - origin of the volume in world space

origin = np.array([0.0,0.0,0.0])

# samplingrate - rate of the sampling

samplingrate = 1.0 

# color transfer function

ctf = "../data/colormaps/Grayscale.orig.cmap"

# opacity transfer function

otf = "../data/colormaps/Grayscale.orig.cmap"

# name of the volume node

volnodename = "1000.int2560"

# min and max values of the data.

low = 0.0
high = 65536.0

# make da volume

gvt.createVolume(volnodename)
gvt.addVolumeTransferFunctions(volnodename,ctf,otf,low,high)
gvt.addVolumeSamples(volnodename,volume,counts,origin,deltas,samplingrate)

# add an instance. 
# An instance needs a transformation matrix. All coords of the volume data
# are in world coords so we give the identity matrix.

mf = np.identity(4)
gvt.addInstance("inst0",volnodename,mf)

# Set up camera and film. 

eye = np.array([512.0,512.0,512.0],dtype=np.float32)
focus = np.array([127.5,127.5,127.5],dtype=np.float32)
fov = 30.0*np.pi/180.0
upVector = np.array([0.0,0.0,1.0],dtype=np.float32)
rayMaxDepth = 1
raySamples = 1
jitterWindowSize = 0.5
camname = "volCam"

gvt.addCamera(camname,eye,focus,upVector,fov,rayMaxDepth,raySamples,jitterWindowSize)

# set up film.

wsize = np.array([512,512],dtype=np.int32)
filmname = "volFilm"
imagename = "PythonVolImage"

gvt.addFilm(filmname,wsize[0],wsize[1],imagename)

# renderer bits ...

rendername = "PythonVolRenderer"

gvt.addRenderer(rendername,adaptertype,schedtype,camname,filmname,TRUE)

# render it

gvt.render(rendername)

# save the image

gvt.writeimage(rendername,imagename)
