# crossover
Simple http server and client example using cpprestsdk

Projects:
	lib: A few useful functions
	nsv_service: library of the REST service
	nsv_service_runner: command line executable for nsv_service
	unit_tests: unit tests
	web_app: web application that connects to a nsv_service and displays results



Setting up environment

	Setup MongoDB
		Create the directory tree C:\data\db and C:\data\log
		Start the server mongod.exe
		Open mongo terminal and load the file dbscript.txt
		Default port is 27017

	Setup Dev environment notes
		Following nuget packages are needed for the solution
			cpprestsdk (casablanca)
			boost (headers)
			boost-vc141 (build for vc++2017)
			googletest

		Setup mongo-cxx-driver
			Download mongo-c-driver, build and install it
				First build libbson project inside mongo-c-driver src directory and then mongo-c-driver
			Download boost libraries (no need to compile)
			Build mongo-cxx-driver using the directories generated in the installation of the previous steps (libbson, mongo-c-driver and boost)
			Configure "Additional Include Directories" and "Additional Library Directories" in Visual Studio
			Add mongocxx.lib and bsoncxx.lib to your library dependencies
			Copy DLLs into vs debug folder or release folder (you can setup a postbuild rule for this)

		Setup wt 4.0.2
			The easiest way is installing the precompiled binaries and configure paths in the proyect "web_app"


		Default root paths for compilation and installation directories:
			Boost    : "C:\boost\boost_1_66_0"
			BSON     : "C:\libbson"
			MongoC   : "C:\mongo-c-driver"
			MongoCXX : "C:\mongo-cxx-driver"
			Wt       : "C:\Wt 4.0.2 msvs2017 x86"
			
		DLL binaries
			Place external dlls under external_dlls folder

