################
####  OpenVision Class Definition Script
################

"OpenVision" locateInDictionaryOf: CoreWorkspace . isNA 
ifTrue: 
   [ 
   ToolKit createSubclass: "OpenVision";

   OpenVision 
   do: [
       ^self define: 'Channel' 
          toBe: (0 asDouble asPointerTo: ^self basicSpecialized newPrototype);
       Channel define: 'whatAmI' toBe: "OpenVision Channel" ;

       ^self specializeAs: "Constant" at: ^self;

       Channel define: 'State' 
          toBe: (0 asPointerTo: Constant basicSpecialized newPrototype);
       Channel State define: 'whatAmI' toBe: "OpenVision Channel State" ;
       Channel define: 'Status' 
          toBe: (0 asPointerTo: Constant basicSpecialized newPrototype);
       Channel Status define: 'whatAmI' toBe: "OpenVision Channel Status" ;
       Channel define: 'Error' 
          toBe: (0 asPointerTo: Constant basicSpecialized newPrototype);
       Channel Error define: 'whatAmI' toBe: "OpenVision Channel Error" ;
       Channel define: 'ChannelType' 
          toBe: (0 asPointerTo: Constant basicSpecialized newPrototype);
       Channel ChannelType
           define: 'whatAmI' toBe: "OpenVision Channel ChannelType" ;
       Channel define: 'Option' 
          toBe: (0 asPointerTo: Constant basicSpecialized newPrototype);
       Channel Option define: 'whatAmI' toBe: "OpenVision Channel Option" ;
   
       Constant specializeAs: "OptionValue" at: ^self;
   
       Channel define: 'PromptFormat' 
          toBe: (0 asPointerTo: OptionValue basicSpecialized newPrototype);
       Channel PromptFormat
            define: 'whatAmI' toBe: "OpenVision Channel PromptFormat" ;
       Channel define: 'BinaryFormat' 
          toBe: (0 asPointerTo: OptionValue basicSpecialized newPrototype);
       Channel BinaryFormat
            define: 'whatAmI' toBe: "OpenVision Channel BinaryFormat" ;
       Channel define: 'TrimFormat' 
          toBe: (0 asPointerTo: OptionValue basicSpecialized newPrototype);
       Channel TrimFormat
            define: 'whatAmI' toBe: "OpenVision Channel TrimFormat" ;
       ];
   ] ;

OpenVision classDescriptor setClassTypeTo: "BuiltIn" ;
Schema addClass: OpenVision Channel at: OpenVision .
  setClassTypeTo: "BuiltIn" ;
Schema addClass: OpenVision Constant at: OpenVision .
  setClassTypeTo: "BuiltIn" ;
Schema addClass: OpenVision Channel State at: OpenVision Channel .
  setClassTypeTo: "BuiltIn" ;
Schema addClass: OpenVision Channel Status at: OpenVision Channel .
  setClassTypeTo: "BuiltIn" ;
Schema addClass: OpenVision Channel Error at: OpenVision Channel .
  setClassTypeTo: "BuiltIn" ;
Schema addClass: OpenVision Channel ChannelType at: OpenVision Channel .
  setClassTypeTo: "BuiltIn" ;
Schema addClass: OpenVision Channel Option at: OpenVision Channel .
  setClassTypeTo: "BuiltIn" ;
Schema addClass: OpenVision OptionValue at: OpenVision .
  setClassTypeTo: "BuiltIn" ;
Schema addClass: OpenVision Channel PromptFormat at: OpenVision Channel .
  setClassTypeTo: "BuiltIn" ;
Schema addClass: OpenVision Channel BinaryFormat at: OpenVision Channel .
  setClassTypeTo: "BuiltIn" ;
Schema addClass: OpenVision Channel TrimFormat at: OpenVision Channel .
  setClassTypeTo: "BuiltIn" ;

