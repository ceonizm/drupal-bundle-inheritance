<?php

/**
 * a bundle can be inherited by many childs bundle 
 * @author François Boukhalfa <francois.boukhalfa@ile-des-medias.com>
 *
 */
class BundleInheritanceRelation extends Entity
{

    const EntityType = "inheritance_relation";

    public $parent;

    public $bundle = self::EntityType;
    
    public function __construct( array $values = array(), $entity_type = self::EntityType )
    {
        parent::__construct( $values, $entity_type );
    }
}

class BundleInheritanceController extends EntityAPIControllerExportable
{
    public function __construct ($entityType = BundleInheritanceRelation::EntityType)
    {
        parent::__construct($entityType);
    }
    
    public function save( $entity, $transaction = NULL )
    {
        $q = new EntityFieldQuery();
        $res = $q->entityCondition('entity_type', $this->entityType)
          ->propertyCondition('parent', $entity->parent)
          ->propertyCondition('child', $entity->child )
          ->execute();
        
        if( is_array( $res ) && (!array_key_exists($this->entityType, $res) ) )
        {
            return parent::save($entity, $transaction );
        }
        return FALSE;
    }

    /**
     * returns the inheritance chain of a bundle
     * 
     * @param unknown $bundle
     * @return multitype:unknown
     */
    public function getInheritanceChain ($bundle)
    {
        $map =& drupal_static( __FUNCTION__);
        if( !isset( $map ) ) $map = array();
        if( !array_key_exists($bundle, $map) )
        {   
            $q = new EntityFieldQuery();
            $result = $q->entityCondition('entity_type', BundleInheritanceRelation::EntityType)
                ->propertyCondition('child', $bundle, "=")
                ->execute();
            if ($result && $result[BundleInheritanceRelation::EntityType]) {
                $chain = array();
                foreach ( $this->load( array_keys( $result[BundleInheritanceRelation::EntityType] ) ) as $relation) {
                    $chain[] = $relation->parent;
                    $parents = $this->getInheritanceChain($relation->parent);
                    
                    if ($parents) {
                        $chain = array_merge($parents, $chain);
                    }
                }
                $map[$bundle] = $chain;
                return $chain;
            }
            $map[$bundle] = NULL;
            return NULL;
        }
        return $map[$bundle];
    }
    
    public function getChilds( $parent )
    {
        $map =& drupal_static( __FUNCTION__);
        if( !isset( $map ) ) $map = array();
        if( !array_key_exists($parent, $map) )
        {
            $q = new EntityFieldQuery();
            $result = $q->entityCondition('entity_type', BundleInheritanceRelation::EntityType)
            ->propertyCondition('parent', $parent, "=")
            ->execute();
            
            if ($result && array_key_exists( BundleInheritanceRelation::EntityType, $result) ) {
                $childs = array();
                foreach( $this->load( array_keys( $result[BundleInheritanceRelation::EntityType] ) ) as $relation)
                {
                     $childs[] = $relation->child;
                }
                $map[$parent] = $childs;
                return $childs;
            }
            $map[$parent] = NULL;
            return NULL;
        }
        return $map[$parent];
    }
}