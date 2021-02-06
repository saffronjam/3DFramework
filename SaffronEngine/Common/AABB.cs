using System;
using System.Diagnostics;
using System.Globalization;
using System.Numerics;

namespace SaffronEngine.Common
{
    public class AABB : IEquatable<AABB>
    {
        /// <summary>Represents an axis-aligned bounding box</summary>
        /// <summary>Minimum point of the bounding box.</summary>
        public readonly Vector3 Min;

        /// <summary>Maximum point of the bounding box.</summary>
        public readonly Vector3 Max;

        /// <summary>Constructs a new AABB.</summary>
        /// <param name="min">Minimum point.</param>
        /// <param name="max">Maximum point.</param>
        public AABB(Vector3 min, Vector3 max)
        {
            this.Min = min;
            this.Max = max;
        }

        /// <summary>Tests equality between two Bounding Boxes.</summary>
        /// <param name="a">First AABB</param>
        /// <param name="b">Second AABB</param>
        /// <returns>True if the Bounding Boxes are equal, false otherwise</returns>
        public static bool operator ==(AABB a, AABB b) => a.Min == b.Min && a.Max == b.Max;

        /// <summary>Tests inequality between two Bounding Boxes.</summary>
        /// <param name="a">First AABB</param>
        /// <param name="b">Second AABB</param>
        /// <returns>True if the Bounding Boxes are not equal, false otherwise</returns>
        public static bool operator !=(AABB a, AABB b) => a.Min != b.Min || a.Max != b.Max;

        /// <summary>
        /// Tests equality between this AABB and another AABB.
        /// </summary>
        /// <param name="other">AABB to test against</param>
        /// <returns>True if components are equal</returns>
        public bool Equals(AABB other)
        {
            Debug.Assert(other != null, nameof(other) + " != null");
            return this.Min == other.Min && this.Max == other.Max;
        }

        /// <summary>
        /// Tests equality between this vector and another object.
        /// </summary>
        /// <param name="obj">Object to test against</param>
        /// <returns>True if the object is a vector and the components are equal</returns>
        public override bool Equals(object obj) => obj is AABB other && this.Equals(other);

        /// <summary>Returns a hash code for this instance.</summary>
        /// <returns>
        /// A hash code for this instance, suitable for use in hashing algorithms and data structures like a hash table.
        /// </returns>
        public override int GetHashCode() => this.Min.GetHashCode() + this.Max.GetHashCode();

        /// <summary>
        /// Returns a <see cref="T:System.String" /> that represents this instance.
        /// </summary>
        /// <returns>
        /// A <see cref="T:System.String" /> that represents this instance.
        /// </returns>
        public override string ToString() => string.Format(CultureInfo.CurrentCulture, "{{Min:{0} Max:{1}}",
            new object[2]
            {
                (object) this.Min.ToString(),
                (object) this.Max.ToString()
            });
    }
}