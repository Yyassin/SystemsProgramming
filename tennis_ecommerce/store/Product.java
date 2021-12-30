/*
 * Authored-by: Yousef Yassin  
 * Last Edited: April 4, 2021
 */
package store;

/**
 * A product with a name, id and price.
 * @author Yousef Yassin  
 * @version 1.0
 */
public class Product {
    private final String NAME;      //this product's name
    private final int ID;           //this product's id (unique)
    private final double PRICE;     //this product's price
    private final String IMAGE_URL; //this product's image (link)

    /**
     * Create a new product with the supplied attributes.
     * @param name String, this product's name.
     * @param id Integer (unique), this product's id.
     * @param price double, this product's price.
     */
    public Product(String name, int id, double price, String imageURL) {
        this.NAME = name;
        this.ID = id;
        this.PRICE = price;
        this.IMAGE_URL = imageURL;
    }

    /**
     * Get the name of this product.
     * @return String, this product's name.
     */
    public String getName() { return this.NAME; }

    /**
     * Get the id of this product.
     * @return Integer (unique), this product's id.
     */
    public int getId() { return this.ID; }

    /**
     * Get the price of this product.
     * @return double, this product's price.
     */
    public double getPrice() { return this.PRICE; }

    /**
     * Get the image url of this product.
     * @return String, this product's image url.
     */
    public String getImageURL() { return this.IMAGE_URL; }

    /**
     * Check if this product is value-equal to the supplied one.
     * @param obj Object, the object (product) to compare.
     * @return boolean, true if products are same, false otherwise.
     */
    @Override
    public boolean equals(Object obj){
        if (obj == null) return false;
        if (obj.getClass() != this.getClass()) return false;

        final Product other = (Product) obj;
        return other.getId() == this.getId() &&         //technically the only one we need.
                other.getPrice() == this.getPrice() &&
                other.getName() == this.getName();
    }
}
