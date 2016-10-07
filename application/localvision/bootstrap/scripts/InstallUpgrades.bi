# 

####################
#  This Script is Used To Install protocol added 
#  using the patch table theory of operation which
#  incrmentally updates included sections of the 
#  protocol
#    
####################

set scriptDir = $bootSource/protocol/upgrades

set comment = "Install upgrades/builtin"
echo "--->" $comment
date

$DBbatchvision -adU3 << @@@EOF@@@

"$scriptDir/builtin/order.builtin" asFileContents evaluate;

Schema processAllMessages ;

Utility updateNetworkWithAnnotation: "$comment" ;
?g

@@@EOF@@@


set comment = "Install upgrades/investment"
echo "--->" $comment
date

$DBbatchvision -adU3 << @@@EOF@@@

"$scriptDir/investment/order.investment" asFileContents evaluate ;

Schema processAllMessages ;

Utility updateNetworkWithAnnotation: "$comment" ;
?g

@@@EOF@@@

set comment = "Install upgrades/datafeed"
echo "--->" $comment
date

$DBbatchvision -adU3 << @@@EOF@@@

"$scriptDir/datafeed/order.datafeed" asFileContents evaluate ;
Schema processAllMessages ;

Utility updateNetworkWithAnnotation: "$comment" ;
?g

@@@EOF@@@
