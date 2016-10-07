####################
# Create feed classes.
####################

# $File: //depot/vision/applications/core/datafeed/mainline/INIfeeds.G $
# $DateTime: 2009/08/13 10:07:27 $
# $Revision: #1 $
# $Author: sglee $


#####-----
#### From patch_104.vis
#####-----


####################
#  Create New subclass of EEF for Indexed feeds and a new subclass of 
#     DataRecord for storing the extra backpointer
####################

(CoreWorkspace respondsTo: "IndexedDataRecord" . not)
ifTrue: [ 

        ## -- Create the class
        #--------------------
        #  IndexedDataRecord/IndexedDataRecordFeed
        #--------------------
        Interface BatchFeedManager createDataRecordClass: "IndexedDataRecord" 
        from: "DataRecord" linkedTo: "Entity" via: "" asTS: "N" ;
  

        #--------------------
        #  Add extra property that represents the index.
        #  Add class constant that indicates the class used for the index.
        #  Note that updateBlock contains the intensional form of the the
        #     message that returns the IndexedList property at the entity
        #     class.
        #--------------------

        ##-- Adds to Schema; and added as upload property for feed, but not 
        ##   expected to be updated via feed. 
        PropertySetup updateFromString: "property | classId
        index | IndexedDataRecord
        ";

        IndexedDataRecord define: 'indexType' toBe: NA ;


        ####################
        #   IndexedDataRecordFeed protocol
        ####################

        #--------------------
        #  Initialization protocol for IndexedDataRecordFeed
        #--------------------

        IndexedDataRecordFeed defineFixedProperty: 'entity' ;
        IndexedDataRecordFeed defineFixedProperty: 'indexEntity' ;

       ];



