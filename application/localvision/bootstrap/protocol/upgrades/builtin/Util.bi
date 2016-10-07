##################
####  Utility ####
##################

#----------
# Utility SessionAttribute
# From core/builtin/patch_124.vis 12/15/2008
#--------------------

Utility SessionAttribute define: 'visionLogTag' toBe: (282 asPointerTo: Utility SessionAttribute) ;


###
# From patch_132.vis
###

Utility define:'_auditFile' toBePrimitive: 606 withControlValue: 0;
Utility define:'_auditEntry' toBePrimitive: 606 withControlValue: 1;
Utility define:'_compilationLogin' toBePrimitive: 606 withControlValue: 2;
Utility define:'_compilationDate' toBePrimitive: 606 withControlValue: 3;
Utility define:'_platform' toBePrimitive: 606 withControlValue: 4;
Utility define:'_release' toBePrimitive: 606 withControlValue: 5;

Utility defineMethod:[|releaseId|
  !releaseSource <- _release;
  releaseSource isString ifTrue: [
    !releaseMarker <-"ReleaseID:";
    !releaseId <- releaseSource drop: (releaseMarker substringOriginIn: releaseSource) + releaseMarker count;
    releaseId count >= 5 ifTrue:[releaseId] ifFalse:[NA]
  ] ifFalse: [NA]
];

Utility defineMethod:[|platformId|
  !platformSource <- _platform;
  platformSource isString ifTrue: [
    !platformMarker <- "Platform:";
    !platformString <- platformSource drop: (platformMarker substringOriginIn:platformSource) + platformMarker count
  ] ifFalse: [NA]
];

Utility defineMethod:[|displayRelease|
  "batchvision Release: " print; releaseId printNL;
  " for: " print; platformId printNL;
];
Utility defineMethod: [|displayAuditInfo|
  _auditFile printNL isntNA ifTrue:[
    _auditEntry printNL;
    _compilationLogin printNL;
    _compilationDate printNL;
    _platform printNL;
    _release printNL;
 ];
];

