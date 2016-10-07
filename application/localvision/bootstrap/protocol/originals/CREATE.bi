##################################################
#  Creates Built In Classes That Do Not Need to be
#    created by the bootstrap
#  Run in OS2
##################################################

"
####################
####  BuiltIn Class Non-Primitive Creation 
####################
" 
?g


################################
####  Date Offset Creation  ####
################################

Prototypical
  define: 'DateOffset'        
    toBe: (0 asPointerTo: Prototypical Ordinal basicSpecialized ) .
  define: 'Days'              
    toBe: (0 asPointerTo: Prototypical DateOffset basicSpecialized ) .
  define: 'BusinessDays'      
    toBe: (0 asPointerTo: Prototypical DateOffset basicSpecialized ) .
  define: 'Months'            
    toBe: (0 asPointerTo: Prototypical DateOffset basicSpecialized ) .
  define: 'MonthBeginnings'   
    toBe: (0 asPointerTo: Prototypical DateOffset basicSpecialized ) .
  define: 'MonthEnds'         
    toBe: (0 asPointerTo: Prototypical DateOffset basicSpecialized ) .
  define: 'Quarters'          
    toBe: (0 asPointerTo: Prototypical DateOffset basicSpecialized ) .
  define: 'QuarterBeginnings' 
    toBe: (0 asPointerTo: Prototypical DateOffset basicSpecialized ) .
  define: 'QuarterEnds'       
    toBe: (0 asPointerTo: Prototypical DateOffset basicSpecialized ) .
  define: 'Years'             
    toBe: (0 asPointerTo: Prototypical DateOffset basicSpecialized ) .
  define: 'YearBeginnings'    
    toBe: (0 asPointerTo: Prototypical DateOffset basicSpecialized ) .
  define: 'YearEnds'          
    toBe: (0 asPointerTo: Prototypical DateOffset basicSpecialized ) .
;

#--------------------

##############################
####  DateRange Creation  ####
##############################
Prototypical
    define: 'DateRange'
      toBe:
     [
         ^current defineMethod:
          [|from: date1 to: date2 by: increment| ^current];
         ^current from: ^today to: ^today by: 0 days
     ] value
;


############################
####  Utility Creation  ####
############################

^current define: 'Utility' toBe: Prototypical Object basicSpecialized ;

Utility
  define: 'UnixSeconds' 
  toBe: (0 asPointerTo: 
          Prototypical Object basicNewPrototype basicSpecializeClass
        ) ;

Utility
define: 'SessionAttribute'
  toBe: (0 asPointerTo: 
          Prototypical Object basicNewPrototype basicSpecializeClass
        ).
;
?g
   
