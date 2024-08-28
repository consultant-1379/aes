
//******************************************************************************
//
// NAME
//      blockpairmappermanager.cpp
//
//  COPYRIGHT Ericsson Utvecklings AB, Sweden 2003.
//  All rights reserved.
//
//  The Copyright to the computer program(s) herein 
//  is the property of Ericsson Utvecklings AB, Sweden.
//  The program(s) may be used and/or copied only with 
//  the written permission from Ericsson Utvecklings AB or in 
//  accordance with the terms and conditions stipulated in the 
//  agreement/contract under which the program(s) have been 
//  supplied.

// .DESCRIPTION 
//  Handles mapping of block-pair objects during runtime for destinations
//  in a block destination set.

// DOCUMENT NO
//  190 89-CAA 109 0507

// AUTHOR 
//  2003-01-14 by UAB/UKY/AU Hans-Erik Nilsson

// SEE ALSO 
//  BlockPairMapper
//
//******************************************************************************

#include <blockpairmappermanager.h>

BlockPairMapperManager *BlockPairMapperManager::instance_ = 0;


//******************************************************************************
//   Constructor
//******************************************************************************
//##ModelId=3E22DCEC01A2
BlockPairMapperManager::BlockPairMapperManager()
{}

//******************************************************************************
//   Destructor
//******************************************************************************
//##ModelId=3E22DCF400FA
BlockPairMapperManager::~BlockPairMapperManager()
{}

//******************************************************************************
//   STATIC: instance()
//******************************************************************************
//##ModelId=3E22DD3C0143
BlockPairMapperManager* BlockPairMapperManager::instance()
{
    if (instance_ == 0)
    {
        instance_ = new BlockPairMapperManager();
    }

    return instance_;
}

//******************************************************************************
//   getMapper()
//******************************************************************************
//##ModelId=3E22DD6B03C2
BlockPairMapper * BlockPairMapperManager::getMapper(const std::string & name)
{
    mapperMX_.acquire();

    BlockPairMapper *ptr = 0;

    std::map< std::string, BlockPairMapper *>::iterator itr = db.end();

    if ((itr = db.find(name)) != db.end())
    {
        ptr = (*itr).second;
    }
    else
    {
        ptr = new BlockPairMapper(name);
        if (BlockPairMapper::init(name) == true)
        {
            db.insert(std::make_pair(name, ptr));
        }
        else
        {
            delete ptr; 
			mapperMX_.release();	
            return NULL;
        }
    }

    mapperMX_.release();

    return ptr;
}

//******************************************************************************
//   remove()
//******************************************************************************
//##ModelId=3E22DDB9002B
void BlockPairMapperManager::remove(const std::string & name)
{
    mapperMX_.acquire();
    std::map< std::string, BlockPairMapper *>::iterator itr = db.end();

    if ((itr = db.find(name)) != db.end())
    {
        BlockPairMapper::remove(name);
        delete (*itr).second;
	(*itr).second=NULL;    
        db.erase(itr);
    }

    mapperMX_.release();
}


//******************************************************************************
//   destroy()
//******************************************************************************
//##ModelId=3E8AAB2401E1
void BlockPairMapperManager::destroy()
{
    mapperMX_.acquire();
    std::map< std::string, BlockPairMapper *>::iterator itr = db.end();

    for(itr = db.begin(); itr != db.end();)
    {
        delete (*itr).second;
	(*itr).second=NULL;    
    	db.erase(itr++);
    }
    mapperMX_.release();
    delete instance_;
    instance_ = 0;
}

