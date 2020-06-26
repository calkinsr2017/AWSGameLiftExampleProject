# Serverless connection to Oculus

This is an example project for doing a simple authorization of Oculus via a login browser in the game as well as

# What you will need

1. Git bash: https://gitforwindows.org/
2. Unreal Engine 4.24 - where you can install it from the Epic Games Launcher. You may need to make an account for this if you don't have one. https://www.epicgames.com/store/en-US/download. From there you can go to Unreal Engine -> Library -> click the plus sign and install Unreal Engine version 4.24.3
3. Oculus download: https://www.oculus.com/setup/
4. Visual Studio 2019 or 2017 community version.

NOTE: This project was only tested with Windows. I am not sure if all the things above can be used with other OSes.

### Steps to get this project setup.
1. Open up git bash through your machine - Windows preferably, haven't tested out other  - and run `git clone https://github.com/calkinsr2017/AWSGameLiftExampleProject.git`
2. Through git bash still, cd into your newest directory made from step (1) and run `git checkout OculusLoginFunctionality`
3. With UE 4.24 installed, right click the ".uproject" file and click "Switch Unreal Engine version" and select 4.24. This will show up if you have installed UE 4.24. Then right click the .uproject again and click "Generate Visual Studio project files." This should generate the folders needed in Visual Studio's side of things.
4. Open up the .sln that gets made and make sure the build environment is set to "Development Editor" and "Win64."
5. Click build solution at the top.
6. Input the Oculus clientID + clientSecretId into a new directory you make called "Urls" in the "Content" folder of this project. 
The ClientID txt file should be called "OculusAppID.txt" and the ClientSecretId txt file should be called "OculusSecretID.txt." 
Also make a txt file called "OculusUrl.txt" and input this: 
`https://auth.oculus.com/sso/?redirect_uri=https://coredisruption.com/oa&organization_id=104556927967406` in that text file.
7. Double click the project to open up and let shaders finish compiling.
8. Open up Oculus to run it and create an account on there if you don't have one already that isn't associated with Facebook.
9. Once that is done you can right click the .uproject and click "Launch Game" and test out the simple authentication + display of your account information! This is
very basic, but it shows that you can use OAuth 2.0 to generate a json data with token information and use it to get your own username out of it. This is according to this: https://developer.oculus.com/documentation/native/ps-account-linking/?locale=en_US

NOTE: For step 9, do not attempt to use the Facebook option to login, that is currently broken. Please use the alternative method where you sign in with the email you signed up Oculus with for step 8. This is a bug we plan to fix eventually.

### TODO 
1. Fix bug with logging in through Facebook in the Oculus website
2. Figure out how to add Test Users to test this build.
3. Set up Oculus as an Identity Provider with a UserInfo endpoint.
4. Test friends + leaderboard through EOS.
