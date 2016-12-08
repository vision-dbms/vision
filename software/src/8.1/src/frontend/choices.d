/****************************************************************************
*****									*****
*****			choices.d					*****
*****									*****
****************************************************************************/
#ifndef CHOICES_D
#define CHOICES_D

typedef struct {
    int	    subfield;
    char    *choices;
    
}  CHOICE_MenuChoice;

#define CHOICES_Subfield(fieldChoice)	    (fieldChoice->subfield)
#define CHOICES_Choices(fieldChoice)	    (fieldChoice->choices)

#define ON	'1'
#define OFF	'0'


#endif
