#**********************************************************
#**********************************************************
#		Test: getenv
#
#  Test StringRefNil: 
#
#**********************************************************
#**********************************************************


Utility echoToFile: (^tmp componentOutput concat:"/getenv.O");
newLine print; "Beginning file: getenv.S ..." printNL;
###########################################################
#		Tests Begin Here ...                      #
###########################################################

!blockStringEnvVars <- 
"SHELL",
"TERM",
"SHLVL",
"GROUP",
"USER",
"HOST"
;

!lstoreStringEnvVars <- blockStringEnvVars send:[fixed];

"Scalar access of an existing env var(using block string):" printNL;
"HOST" do:[print;"=" print; getenv printNL];
newLine print;

"Scalar access of a non-existing env var(using block string):" printNL;
"user" do:[print;"=" print; getenv printNL];
newLine print;

"Scalar access of an existing env var(using lstore string):" printNL;
"HOST" fixed do:[print;"=" print; getenv printNL];
newLine print;

"Scalar access of a non-existing env var(using lstore string):" printNL;
"user" fixed do:[print;"=" print; getenv printNL];
newLine print;

"List access of existing env vars (block string storage):" printNL;
blockStringEnvVars do:[print;"=" print; getenv printNL];
newLine print;

"List access of existing env vars (lstore string storage):" printNL;
lstoreStringEnvVars do:[print;"=" print; getenv printNL];
newLine print;

!blockStringEnvVarsAndBogusVars <-
"XXXyyyZZZ",
"SHELL",
"XXXyyyZZZ",
"XXXyyyZZZ",
"TERM",
"HOSTTYPE",
"XXXyyyZZZ",
"VENDOR",
"OSTYPE",
"MACHTYPE",
"SHLVL",
"XXXyyyZZZ",
"GROUP",
"HOST",
"XXXyyyZZZ",
"XXXyyyZZZ",
"HOME",
"LocalVisionRoot",
"XXXyyyZZZ",
"XXXyyyZZZ",
"XXXyyyZZZ",
"USER",
"XXXyyyZZZ",
"XXXyyyZZZ",
"XXXyyyZZZ",
"XXXyyyZZZ",
"XXXyyyZZZ"
;

!lstoreStringEnvVarsAndBogusVars <- blockStringEnvVarsAndBogusVars send:[fixed];

"List access of mixed existing and bogus env vars (block string storage):" printNL;
blockStringEnvVarsAndBogusVars do:[print;"=" print; getenv printNL];
newLine print;

"List access of mixed existing and bogus env vars (lstore string storage):" printNL;
lstoreStringEnvVarsAndBogusVars do:[print;"=" print; getenv printNL];


###########################################################
#		End of tests ...			  #
###########################################################
newLine print; "End of file: getenv.S" printNL; "" printNL;
?g
?t
Utility stopEchoToFile;
?g

