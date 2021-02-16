using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using SaffronEngine.Exceptions;

namespace SaffronEngine.Common
{
    public class Entity
    {
        public class Registry
        {
            public Dictionary<Guid, Entity> Entities { get; private set; }
            private readonly Dictionary<Type, HashSet<Entity>> _entityGroups;
            private readonly Dictionary<Type, List<ISaffronComponent>> _components;
            private readonly Dictionary<Entity, Dictionary<Type, ISaffronComponent>> _ownerShips;
            private readonly HashSet<Entity> _emptyEntityHashSet;
            private readonly List<ISaffronComponent> _emptyComponentList;

            public Registry()
            {
                Entities = new Dictionary<Guid, Entity>();

                _entityGroups = new Dictionary<Type, HashSet<Entity>>();
                _components = new Dictionary<Type, List<ISaffronComponent>>();
                _ownerShips = new Dictionary<Entity, Dictionary<Type, ISaffronComponent>>();

                _emptyEntityHashSet = new HashSet<Entity>();
                _emptyComponentList = new List<ISaffronComponent>();
            }

            public Entity Add(Entity entity)
            {
                Entities.Add(entity.Handle, entity);
                entity.AssignRegistry(this);
                return entity;
            }

            public E AddComponent<E>(Entity entity) where E : ISaffronComponent, new()
            {
                if (HasComponent<E>(entity))
                {
                    return GetComponent<E>(entity);
                }

                // If no entity has this component
                // Then allocate space for it
                if (!ComponentRegistered(typeof(E)))
                {
                    _entityGroups.Add(typeof(E), new HashSet<Entity>());
                    _components.Add(typeof(E), new List<ISaffronComponent>());
                }

                _entityGroups[typeof(E)].Add(entity);

                // If entity did not have any previous components
                // Then allocate space for it
                if (!_ownerShips.ContainsKey(entity))
                {
                    _ownerShips.Add(entity, new Dictionary<Type, ISaffronComponent>());
                }

                // Finally create the requested component
                var component = new E();
                _components[typeof(E)].Add(component);
                _ownerShips[entity].Add(typeof(E), component);

                return component;
            }

            public E GetComponent<E>(Entity entity) where E : ISaffronComponent
            {
                Debug.Assert(Entities.ContainsKey(entity.Handle), "Entity was not registered");
                Debug.Assert(ComponentRegistered(typeof(E)) && _entityGroups[typeof(E)].Contains(entity),
                    "Entity did not have component");

                return (E) _ownerShips[entity][typeof(E)];
            }

            public bool HasComponent<E>(Entity entity) where E : ISaffronComponent
            {
                return ComponentRegistered(typeof(E)) && _entityGroups[typeof(E)].Contains(entity);
            }

            public IEnumerable<Entity> AllWith(params dynamic[] components)
            {
                var result = new List<Entity>();
                foreach (var entity in Entities.Values)
                {
                    Debug.Assert(_ownerShips.ContainsKey(entity), "Internal error, entity in list, but not registered");
                    if (components.All(component => _ownerShips[entity].ContainsKey(component)))
                    {
                        result.Add(entity);
                    }
                }
                return result;
            }

            public List<ISaffronComponent> All<E>() where E : ISaffronComponent
            {
                return ComponentRegistered(typeof(E)) ? _components[typeof(E)] : _emptyComponentList;
            }

            private bool ComponentRegistered(Type type)
            {
                return _entityGroups.ContainsKey(type);
            }

            private IEnumerable<Entity> GetGroupFromType(Type type)
            {
                return _entityGroups[type];
            }
        }

        public Guid Handle { get; private set; }
        public static Entity Null => new Entity();
        private Registry _registry = null;


        public Entity(Guid handle = new Guid())
        {
            Handle = handle;
        }

        public Entity()
        {
            Handle = Guid.NewGuid();
        }

        private void AssignRegistry(Registry registry)
        {
            if (_registry != null)
            {
                throw new SaffronStateException("Tried to assign registry two times");
            }

            _registry = registry;
        }


        public E AddComponent<E>() where E : ISaffronComponent, new()
        {
            return _registry.AddComponent<E>(this);
        }

        public E GetComponent<E>() where E : ISaffronComponent
        {
            return _registry.GetComponent<E>(this);
        }

        public bool HasComponent<E>() where E : ISaffronComponent
        {
            return _registry.HasComponent<E>(this);
        }
    }
}