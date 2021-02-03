using System;
using System.Collections.Generic;

namespace SaffronEngine.Common
{
    public class Entity
    {
        public class Registry
        {
            public Dictionary<Guid, Entity> Entities { get; private set; }
            private Dictionary<Type, HashSet<Entity>> _entityGroups = new Dictionary<Type, HashSet<Entity>>();
            private readonly HashSet<Entity> _emptyList = new HashSet<Entity>();

            public Registry()
            {
                Entities = new Dictionary<Guid, Entity>();
            }

            public void Add(Entity entity)
            {
                Entities.Add(entity.Handle, entity);
            }

            public HashSet<Entity> AllWith(params dynamic[] components)
            {
                switch (components.Length)
                {
                    case 0:
                        return _emptyList;
                    case 1:
                        return _entityGroups.ContainsKey(components[0]) ? _entityGroups[components[0]] : _emptyList;
                    default:
                    {
                        var result = new HashSet<Entity>();
                        foreach (var component in components)
                        {
                            if (TypeRegistered(component))
                            {
                                result.UnionWith(GetGroupFromType(component));
                            }
                        }

                        return result;
                    }
                }
            }

            private bool TypeRegistered(Type type)
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

        public Entity(Guid handle = new Guid())
        {
            Handle = handle;
        }

        public Entity()
        {
        }
    }
}