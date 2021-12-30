/*
 * Authored-by: Yousef Yassin  
 * Last Edited: April 4, 2021
 */
package store;

import javax.imageio.ImageIO;
import javax.swing.*;
import javax.swing.text.NumberFormatter;
import java.awt.*;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.io.IOException;
import java.net.URL;
import java.text.NumberFormat;
import java.util.HashMap;
import java.util.LinkedHashMap;


/**
 * Class managing the graphical user-interface of an e-commerce store. The Store View provides functionality to
 * display the store's products and commands to interface with its contents.
 * @author Yousef Yassin  
 * @version 2.0
 */
public class StoreView {
    private final StoreManager sm;          //the store manager managing this view
    private final int CART_ID;              //the id associated with this view (this user)
    private final JFrame frame;             //the application frame

    /**
     * Create a new StoreView instance with the supplied StoreManager.
     * @param sm StoreManager, the manager of this StoreView.
     */
    public StoreView(StoreManager sm) {
        this.sm = sm;
        this.CART_ID = this.sm.generateCartID();
        this.frame = new JFrame();
        this.initFrame();
    }

    /**
     * Display a confirmation before quitting.
     */
    private static void quit(JFrame frame) {
        if (JOptionPane.showConfirmDialog(frame, "Are you sure you want to quit?")
                == JOptionPane.OK_OPTION) {
            frame.setVisible(false);
            frame.dispose();
        }
    }

    /**
     * Initialize the application frame.
     */
    private void initFrame() {
        this.frame.setTitle("The Tennis Store");
        this.frame.setIconImage(Toolkit.getDefaultToolkit().getImage(getClass().getResource("icon.png")));
        this.frame.add(this.mainPanel());
        this.frame.pack();

        //Prompt on quit. Credit: Martin Klamrowski, Cristina Ruiz Martin
        this.frame.setDefaultCloseOperation(JFrame.DO_NOTHING_ON_CLOSE);
        this.frame.addWindowListener(new WindowAdapter() {

            /**
             * Displays quit confirmation on window closing event.
             * @param we WindowEvent, the window closing event.
             */
            @Override
            public void windowClosing(WindowEvent we) { quit(frame); }
        });
    }

    /**
     * Creates primary container panel to wrap all other store panels: menu
     * and store.
     * @return JPanel, the main panel.
     */
    private JPanel mainPanel() {
        JPanel mainPanel = new JPanel(new BorderLayout());
        mainPanel.add(this.menuPanel(), BorderLayout.LINE_START);

        //Terminate if image URLs fail
        try {
            mainPanel.add(this.storePanel(), BorderLayout.CENTER);
        } catch (IOException e) {
            e.printStackTrace();
        }

        return mainPanel;
    }

    /**
     * Creates fixed-left menu panel containing primary application navigation:
     * viewing cart, checking out and quitting.
     * @return JPanel, the menu panel.
     */
    private JPanel menuPanel() {
        String bg = "#333333";
        String fg = "#ffffff";

        JPanel menuPanel = new JPanel(new GridBagLayout());
        menuPanel.setPreferredSize(new Dimension(250, 600));
        menuPanel.setBackground(Color.decode(bg));

        //Menu buttons
        JButton cartButton = JFlatButton("Cart", bg, fg);
        JButton checkoutButton = JFlatButton("Checkout", bg, fg);
        JButton quitButton = JFlatButton("Quit", bg, fg);
        //display the cart without checking out
        cartButton.addActionListener(e -> this.displayCart(false));
        //display the cart and checkout
        checkoutButton.addActionListener(e -> this.displayCart(true));
        //quit the application
        quitButton.addActionListener(e -> quit(this.frame));

        //Vertical button container
        Box box = Box.createVerticalBox();
        box.add(cartButton);
        box.add(Box.createVerticalStrut(10));
        box.add(checkoutButton);
        box.add(Box.createVerticalStrut(10));
        box.add(quitButton);

        JLabel welcomeLabel = new JLabel("Welcome to the Tennis Store!");
        welcomeLabel.setForeground(Color.white);
        welcomeLabel.setFont(new Font("Sans-serif", Font.PLAIN, 18));

        //Position welcome above options, in the center of the menu
        GridBagConstraints c = new GridBagConstraints();
        GridBagConstraints c2 = new GridBagConstraints();
        c.gridy = 0;
        c.gridx = 1;

        c2.gridy = 1;
        c2.gridx = 1;

        menuPanel.add(welcomeLabel, c);
        menuPanel.add(box, c2);
        return menuPanel;
    }

    /**
     * Creates primary store panel to contain all product listings within a
     * scroll view.
     * @return JScrollPane, the scrollable store panel.
     * @throws IOException, in case of malformed product image URL.
     */
    private JScrollPane storePanel() throws IOException {
        JPanel storePanel = new JPanel();
        storePanel.setPreferredSize(new Dimension(400, 1200));
        storePanel.setBackground(Color.decode("#ffffff"));

        //Get inventory products
        LinkedHashMap<Product, Integer> productStock = this.sm.getInventoryInfo();

        //Create a scrollable listing for each product
        for (Product product : productStock.keySet()){
            storePanel.add(this.productCard(product.getName(),
                    product.getPrice(), productStock.get(product), product.getImageURL(), product.getId()));
        }
        JScrollPane scrollableStore = new JScrollPane(storePanel);
        scrollableStore.setPreferredSize(new Dimension(500, 580));

        return scrollableStore;
    }

    /**
     * A product listing card. Displays product information, a visual representation and purchase field.
     * @param name String, the name of the product.
     * @param price double, the price of the product.
     * @param stock int, the remaining stock of the product.
     * @param imageURL String, the product thumbnail's image location.
     * @param productID int, this product's id.
     * @return JPanel, the product listing card panel.
     * @throws IOException, in case of malformed product image URL.
     */
    private JPanel productCard(String name, double price, int stock, String imageURL, int productID) throws IOException {
        JPanel productCard = new JPanel(new BorderLayout());
        productCard.setPreferredSize(new Dimension(300, 200));
        productCard.setBackground(Color.decode("#eeeeeee"));

        JLabel stockLabel = new JLabel(String.valueOf(stock));

        //Horizontal meta
        Box box = Box.createHorizontalBox();
        box.add(new JLabel("CAD " + price + "   " + name + "  | Remaining:  "));
        box.add(stockLabel);

        productCard.add(box, BorderLayout.PAGE_START);
        //set product image
        productCard.add(new JLabel(
                new ImageIcon(new ImageIcon(ImageIO.read(new URL(imageURL))).getImage()
                        .getScaledInstance(300, 200, Image.SCALE_AREA_AVERAGING))
        ));

        //Add buying/returning options
        productCard.add(this.purchaseField(productID, stockLabel), BorderLayout.PAGE_END);

        return productCard;
    }

    /**
     * A product listing's purchase field: input for quantity and buttons for add/remove.
     * @param productID int, the product id corresponding to this field.
     * @param stockLabel JLabel, the stock label corresponding to the product card of this field.
     * @return JPanel, the panel containing this purchase field.
     */
    private JPanel purchaseField(int productID, JLabel stockLabel) {
        JPanel purchaseField = new JPanel(new BorderLayout());
        purchaseField.setPreferredSize(new Dimension(300, 20));

        //Create integer input field
        JFormattedTextField quantityField = intInput();

        purchaseField.add(quantityField, BorderLayout.LINE_START);
        //create add button
        purchaseField.add(this.purchaseButton(true, productID, stockLabel, quantityField)
                , BorderLayout.CENTER);
        //create remove button
        purchaseField.add(this.purchaseButton(false, productID, stockLabel, quantityField),
                BorderLayout.LINE_END);

        return purchaseField;
    }

    /**
     * Displays external pane with cart information. In case of checkout event, terminates
     * application on confirmation.
     * @param checkout boolean, true if checkout even, false otherwise.
     */
    private void displayCart(boolean checkout){
        HashMap<Product, Integer> cartItems = this.sm.getCartInfo(this.CART_ID);

        if (cartItems.isEmpty()) {
            JOptionPane.showMessageDialog(null, "Cart is Empty!", "Error", JOptionPane.ERROR_MESSAGE);
            return;
        }
        if (checkout) {
            if (JOptionPane.showConfirmDialog(frame, "Are you sure you want to checkout?")
                    != JOptionPane.OK_OPTION) return;
        }

        //Create column labels
        JPanel columns = new JPanel(new BorderLayout());
        columns.add(new JLabel("Quantity | "), BorderLayout.LINE_START);
        columns.add(new JLabel("Product Name      | "), BorderLayout.CENTER);
        columns.add(new JLabel("Unit Price"), BorderLayout.LINE_END);

        Box box = Box.createVerticalBox();
        box.add(columns);

        //Row for each product in cart
        for (Product product : cartItems.keySet()){
            JPanel labelPanel = new JPanel(new BorderLayout());
            labelPanel.add(new JLabel("             " + cartItems.get(product) + " |  "), BorderLayout.LINE_START);
            labelPanel.add(new JLabel(product.getName()), BorderLayout.CENTER);
            labelPanel.add(new JLabel("  | CAD " + product.getPrice()), BorderLayout.LINE_END);
            box.add(labelPanel);
        }

        //Empty space
        box.add(new JPanel());

        //Transaction total
        JPanel totalPanel = new JPanel(new BorderLayout());
        totalPanel.add(new JLabel(String.format("Total: CAD %.2f", this.sm.getCartTotal(this.CART_ID))));
        box.add(totalPanel);

        //Show cart contents
        JOptionPane.showMessageDialog(null, box, checkout  ? "Order Summary" : "Cart", JOptionPane.INFORMATION_MESSAGE);

        if (checkout) {
            this.sm.checkout(this.CART_ID);
            this.frame.setVisible(false);
            this.frame.dispose();
        }
    }

    /**
     * Displays main store view graphical interface: menu panel with store options
     * and store panel with product listings.
     */
    public void displayGUI() { this.frame.setVisible(true); }

    /**
     * Initializes the store's inventory with products.
     * @return StoreManager, the initialized store's manager.
     */
    private static StoreManager initStore(){
        HashMap<Product, Integer> products = new HashMap<>();
        String[] names = new String[]{"Rafael Nadal", "Novak Djokovic", "Roger Federer",
                "Stef Tsitsipas", "Nick Kygrgios"};

        String[] imgURLs = new String[]{
                "https://i.guim.co.uk/img/media/0a145fdb57e58da664c26ac7f23143bb4b6e6de5/0_172_4070_2442/master/4070.jpg?width=1200&quality=85&auto=format&fit=max&s=e2e50b38a75d7734db1e0d5ffba730eb",
                "https://phantom-marca.unidadeditorial.es/c9421be4b65e4cdb4773e643eac6d789/crop/0x0/2048x1148/resize/1320/f/jpg/assets/multimedia/imagenes/2021/03/22/16164392678153.jpg",
                "https://www.tennisworldusa.org/imgb/96946/breaking-news-roger-federer-confirmed-for-australian-open-2021.jpg",
                "https://static01.nyt.com/images/2020/11/16/multimedia/16sp-atp-tsitsipas-inyt1/merlin_179522271_b1b3346a-c7e9-4154-98f6-e1466d5d2181-mobileMasterAt3x.jpg",
                "https://www.gannett-cdn.com/presto/2021/02/08/USAT/1ff0d797-a6f3-441e-9d80-4294e7feba73-AP_Australian_Open_Tennis.jpg"
        };

        //Add products to store
        for (int i = 0; i < names.length; i++){
            products.put(new Product(names[i], i, 150 * i + 0.99, imgURLs[i]), i * 5 + 5);
        }

        //Initialize the Store manager
        return new StoreManager(products);
    }

    /**
     * Create a button with a flat design and the supplied text and colours.
     * @param text String, the button's text.
     * @param bg String, hex code of button's colour.
     * @param fg String, hex code of the button's text colour.
     * @return JButton, the flat button.
     */
    private static JButton JFlatButton(String text, String bg, String fg) {
        JButton button = new JButton(text);
        button.setBackground(Color.decode(bg));
        button.setForeground(Color.decode(fg));

        return button;
    }

    /**
     * Creates a purchase event button: add or remove.
     * @param add boolean, true if this is an add button, false otherwise (remove button).
     * @param productID int, the product id corresponding to purchase event.
     * @param stockLabel JLabel, the stock label corresponding to the product card of this button.
     * @param inputField JFormattedTextField, the input field corresponding to the product card of this button.
     * @return JButton, the purchase button.
     */
    private JButton purchaseButton(boolean add, int productID, JLabel stockLabel, JFormattedTextField inputField){
        JButton button = JFlatButton(add ? "Add" : "Remove", "#333333", "#ffffff");
        button.addActionListener(e -> {
            boolean success = add ?
                    this.sm.addToCart(this.CART_ID, productID, (Integer) inputField.getValue())
                    :
                    this.sm.removeFromCart(this.CART_ID, productID, (Integer) inputField.getValue());

            if (!success) {
                JOptionPane.showMessageDialog(null, "Invalid Quantity Entered!", "Error", JOptionPane.ERROR_MESSAGE);
                return;
            }
            stockLabel.setText(String.valueOf(this.sm.getStock(productID)));
            inputField.setValue(0);
        });

        return button;
    }

    /**
     * Creates a formatted text field that only accepts integers.
     * @return JFormattedTextField, the formatted text field.
     */
    private static JFormattedTextField intInput() {
        NumberFormat format = NumberFormat.getInstance();
        NumberFormatter formatter = new NumberFormatter(format) {

            /**
             * Set caret to final position on focus.
             * Source: https://stackoverflow.com/questions/2201873/jformattedtextfield-caret-position-on-focus
             * @param ftf JFormattedTextField, the formatted text field
             */
            @Override
            public void install(final JFormattedTextField ftf) {
                int prevLen = ftf.getDocument().getLength();
                int savedCaretPos = ftf.getCaretPosition();
                super.install(ftf);
                if (ftf.getDocument().getLength() == prevLen) {
                    ftf.setCaretPosition(savedCaretPos);
                }
            }
        };
        formatter.setValueClass(Integer.class);

        JFormattedTextField quantityField = new JFormattedTextField(formatter); //all non-int map to 0
        quantityField.setPreferredSize(new Dimension(120, 20));
        quantityField.setValue(0);

        return quantityField;
    }

    /**
     * Application main script to initialize and display the store view interface.
     * @param args String[], default parameter for Java's main method.
     */
    public static void main(String[] args) {
        StoreView sv = new StoreView(initStore());
        sv.displayGUI();
    }
}
